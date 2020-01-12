// ff-test-adv
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_haptic.h>

#include <stdio.h>              /* printf */
#include <string.h>             /* strstr */
#include <ctype.h>              /* isdigit */

#include <stdbool.h>		/* bool */
#include <math.h>
#include <sys/poll.h>

#include "ff-test-adv.h"


const char axes[AXES] = {'x', 'y', 'z'};

int num_devices;
static hapticDevice *devices = NULL;

int effectId[EFFECTS];
SDL_HapticEffect effect[EFFECTS];

extern struct effect_req_cap effect_capabilities[BUILT_EFFECTS];


/*
 * prototypes
 */
static void abort_execution(int retcode);

static void print_capabilities(int supported)
{
    if (supported & SDL_HAPTIC_CONSTANT)
        printf("      Constant force\n");
    if (supported & SDL_HAPTIC_SINE)
        printf("      Sine wave\n");
#if defined(SDL_HAPTIC_SQUARE)
    if (supported & SDL_HAPTIC_SQUARE)
        printf("      Square wave\n");
#endif
    if (supported & SDL_HAPTIC_TRIANGLE)
        printf("      Triangle wave\n");
    if (supported & SDL_HAPTIC_SAWTOOTHUP)
        printf("      Sawtooth up\n");
    if (supported & SDL_HAPTIC_SAWTOOTHDOWN)
        printf("      Sawtooth down\n");
    if (supported & SDL_HAPTIC_RAMP)
        printf("      Ramp\n");
    if (supported & SDL_HAPTIC_FRICTION)
        printf("      Friction\n");
    if (supported & SDL_HAPTIC_SPRING)
        printf("      Spring\n");
    if (supported & SDL_HAPTIC_DAMPER)
        printf("      Damper\n");
    if (supported & SDL_HAPTIC_INERTIA)
        printf("      Intertia\n");
    if (supported & SDL_HAPTIC_CUSTOM)
        printf("      Custom\n");
    printf("   Supported capabilities:\n");
    if (supported & SDL_HAPTIC_GAIN)
        printf("      Gain\n");
    if (supported & SDL_HAPTIC_AUTOCENTER)
        printf("      Autocenter\n");
    if (supported & SDL_HAPTIC_STATUS)
        printf("      Status\n");
}


/*
 * Displays information about the haptic device.
 */
static void HapticPrintSupported(SDL_Haptic * haptic)
{
    unsigned int supported;

    supported = SDL_HapticQuery(haptic);
    printf("   Supported effects [%d effects, %d simultaneously playing]:\n",
           SDL_HapticNumEffects(haptic), SDL_HapticNumEffectsPlaying(haptic));

	print_capabilities(supported);
}



int init_haptic(void)
{
    /* Initialize the force feedbackness */
    SDL_Init(SDL_INIT_TIMER | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC);

    num_devices = SDL_NumHaptics();
    printf("%d Haptic devices detected.\n", num_devices);

    devices = (hapticDevice*)malloc(num_devices * sizeof(hapticDevice));
    if(!devices) {
        printf("Fatal error: Could not allocate memory for devices!\n");
        abort();
    }

    // Zero
    memset(devices, 0, num_devices * sizeof(hapticDevice));

    // Initialize devices
    for(int i=0;i < num_devices; i++)
    {
        devices[i].num = i;
        devices[i].device = SDL_HapticOpen(i);

        if(devices[i].device) {
          devices[i].open = true;

          // Copy device's name with ascii
          const char *p = SDL_HapticName(i);
          strncpy(devices[i].name, p, NAMELEN);

          // Add device number after name, if there is multiples with same name
          for(int a=0; a < i; a++) {
               if(strcmp(devices[i].name, devices[a].name) == 0) {
                   size_t len = strlen(devices[i].name);
                   if(len < NAMELEN - 2) { // Enough space to add number after name
                       devices[i].name[len] = ' ';
                       devices[i].name[len+1] = '0' + i;
                   } else {
                       devices[i].name[NAMELEN-2] = ' ';
                       devices[i].name[NAMELEN-1] = '0' + i;
                   }
               }
          }

          printf("Device %d: %s\n", devices[i].num, devices[i].name);

          // Capabilities
          devices[i].supported = SDL_HapticQuery(devices[i].device);
          devices[i].axes = SDL_HapticNumAxes(devices[i].device);
          devices[i].numEffects = SDL_HapticNumEffects(devices[i].device);
          devices[i].numEffectsPlaying = SDL_HapticNumEffectsPlaying(devices[i].device);
          SDL_HapticSetGain(devices[i].device, 100);

          // Test device support
          HapticPrintSupported(devices[i].device);

        } else {
            printf("Unable to open haptic devices %d: %s\n", i, SDL_GetError());
            devices[i].open = false;
        }
    }

    /* We only want force feedback errors. */
    SDL_ClearError();
	return num_devices;
}

/* Forward declaration from separate file */
void create_haptic_effect(SDL_Haptic *haptic, int effect_num);

void create_effect(int devnum, int effnum)
{
	int i;

	if(devnum < 0 || devnum >= num_devices)
		return;

	for(i=0;i<EFFECTS;i++) {
		if(effectId[i] >= 0) {
			SDL_HapticDestroyEffect(devices[devnum].device, effectId[i]);
			effectId[i] = -1;
		}

		SDL_memset(&effect[i], 0, sizeof(SDL_HapticEffect));
	}

	create_haptic_effect(devices[devnum].device, effnum);
}

void play_effect(int devnum, int effnum)
{
	int i;

	/* Start all effects that are created for this effect */
	for(i=0;i<EFFECTS;i++) {
		if(effectId[i] >= 0) {
			SDL_HapticRunEffect(devices[devnum].device, effectId[i], 1);
		}
	}
}

void stop_and_delete(int devnum)
{
	int i;

	SDL_HapticStopAll(devices[devnum].device);

	/* Start all effects that are created for this effect */
	for(i=0;i<EFFECTS;i++) {
		if(effectId[i] >= 0) {
			SDL_HapticDestroyEffect(devices[devnum].device, effectId[i]);
			effectId[i] = -1;
		}
		SDL_memset(&effect[i], 0, sizeof(SDL_HapticEffect));
	}
}


int select_device()
{
	int i;
	int selected_device = 0;

	do {
		if(num_devices > 1) {
			printf("Select device (-1 to quit): ");
			i = scanf("%d", &selected_device);

			if(i != 1 || selected_device < 0)
				return -1;

		} else if(num_devices == 1) {
			selected_device = 0;
		} else return -1;

		if(selected_device >= num_devices) {
			printf("Erroneous device, only %d devices detected.\n", num_devices);
		}
	} while(selected_device >= num_devices);

	return selected_device;
}

int select_effect(int devnum)
{
	int i;
	int selected_effect;
	bool effect_supported[BUILT_EFFECTS] = {false};

	printf("Supported effects:\n");
	for(i=0;i<BUILT_EFFECTS;i++) {
		if((devices[devnum].supported & effect_capabilities[i].mask) == effect_capabilities[i].mask &&
			devices[devnum].numEffectsPlaying >= effect_capabilities[i].neff) {
			printf("%d ", i);
			effect_supported[i] = true;
		}
	}
	printf("\n");

	while(1) {
		printf("Select effect, -1 to ");
		if(num_devices > 1)
			printf("return to device selection: ");
		else
			printf("quit: ");

		i = scanf("%d", &selected_effect);
		if(i < 1 || selected_effect < 0)
			return -1;

		if(selected_effect >= BUILT_EFFECTS) {
			printf("Erroneous effect number.\n");
			continue;
		} else if(!effect_supported[selected_effect]) {
			printf("Effect not supported, requires:\n");
			print_capabilities(effect_capabilities[selected_effect].mask);
			continue;
		} else {
			return selected_effect;
		}
	}

	return -1;
}

/**
 * @brief The entry point of this force feedback demo.
 * @param[in] argc Number of arguments.
 * @param[in] argv Array of argc arguments.
 */
int main(int argc, char **argv)
{
	int selected_device = 0;
	int selected_effect = 0;

    printf("ff-test-adv version 0.1\n");
    printf("Force feedback advanced effects test\n");
    printf("Copyright 2020 Lauri Peltonen, released under GPLv3 or later\n\n");

    // Initialize SDL haptics
    if(!init_haptic())
	{
		printf("No force-feedback devices detected.\n");
		abort_execution(0);
	}

    // Main loop

    while(1)
    {
		selected_device = select_device();
		if(selected_device < 0)
			break;

		selected_effect = select_effect(selected_device);
		if(selected_effect < 0) {
			if(num_devices > 1)
				continue;

			break;
		}

		create_effect(selected_device, selected_effect);
		play_effect(selected_device, selected_effect);
    }

	abort_execution(0);

    return 0;
}


/*
 * Clean up a bit.
 */
static void abort_execution(int retcode)
{
    printf("\nAborting program execution.\n");


    // Close haptic devices
    for(int i=0; i < num_devices; i++) {
		stop_and_delete(i);
		if(devices[i].open && devices[i].device) SDL_HapticClose(devices[i].device);
	}

    if(devices) free(devices);
    devices = NULL;

    SDL_Quit();

    exit(retcode);
}

