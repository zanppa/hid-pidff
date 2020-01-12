#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_haptic.h>

#include "ff-test-adv.h"

#if !defined(SDL_HAPTIC_SQUARE)
#define SDL_HAPTIC_SQUARE SDL_HAPTIC_SINE
#endif

extern int effectId[EFFECTS];
extern SDL_HapticEffect effect[EFFECTS];

struct effect_req_cap effect_capabilities[BUILT_EFFECTS] = {
    {3, SDL_HAPTIC_SINE | SDL_HAPTIC_CONSTANT},
    {3, SDL_HAPTIC_SINE | SDL_HAPTIC_SQUARE},
    {2, SDL_HAPTIC_SQUARE},
    {3, SDL_HAPTIC_SQUARE | SDL_HAPTIC_SINE},
    {4, SDL_HAPTIC_TRIANGLE | SDL_HAPTIC_SINE},
    {6, SDL_HAPTIC_SQUARE},
    {2, SDL_HAPTIC_SQUARE | SDL_HAPTIC_SINE},
    {10, SDL_HAPTIC_SAWTOOTHUP | SDL_HAPTIC_SQUARE | SDL_HAPTIC_SINE},
    {10, SDL_HAPTIC_SINE | SDL_HAPTIC_SQUARE},
    {10, SDL_HAPTIC_SINE | SDL_HAPTIC_CONSTANT | SDL_HAPTIC_TRIANGLE | SDL_HAPTIC_SQUARE},
    {3, SDL_HAPTIC_SQUARE | SDL_HAPTIC_TRIANGLE},
    {3, SDL_HAPTIC_SQUARE | SDL_HAPTIC_SINE}
    };


void create_haptic_effect(SDL_Haptic *haptic, int effect_num)
{
	switch(effect_num) {
	case 0:
		effect[0].type = SDL_HAPTIC_SINE;
		effect[0].periodic.length = 1000;
		effect[0].periodic.delay = 0;
		effect[0].periodic.button = 0;
		effect[0].periodic.interval = 0;
		effect[0].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[0].periodic.direction.dir[0] = 3100;
		effect[0].periodic.period = 45;
		effect[0].periodic.magnitude = 3612;
		effect[0].periodic.offset = 0;
		effect[0].periodic.phase = 0;
		effect[0].periodic.attack_length = 216;
		effect[0].periodic.attack_level = 10836;
		effect[0].periodic.fade_length = 534;
		effect[0].periodic.fade_level = 1548;
		effectId[0] = SDL_HapticNewEffect(haptic, &effect[0]);
		if(effectId[0] < 0) printf("Error creating effect 0 \n\t%s\n", SDL_GetError());

		effect[1].type = SDL_HAPTIC_CONSTANT;
		effect[1].constant.length = 750;
		effect[1].constant.delay = 0;
		effect[1].constant.button = 0;
		effect[1].constant.interval = 0;
		effect[1].constant.direction.type = SDL_HAPTIC_POLAR;
		effect[1].constant.direction.dir[0] = 0;
		effect[1].constant.level = 32767;
		effect[1].constant.attack_length = 0;
		effect[1].constant.attack_level = 32767;
		effect[1].constant.fade_length = 565;
		effect[1].constant.fade_level = 0;
		effectId[1] = SDL_HapticNewEffect(haptic, &effect[1]);
		if(effectId[1] < 0) printf("Error creating effect 1 \n\t%s\n", SDL_GetError());

		effect[2].type = SDL_HAPTIC_CONSTANT;
		effect[2].constant.length = 750;
		effect[2].constant.delay = 0;
		effect[2].constant.button = 0;
		effect[2].constant.interval = 0;
		effect[2].constant.direction.type = SDL_HAPTIC_POLAR;
		effect[2].constant.direction.dir[0] = 12700;
		effect[2].constant.level = 0;
		effect[2].constant.attack_length = 256;
		effect[2].constant.attack_level = 32767;
		effect[2].constant.fade_length = 476;
		effect[2].constant.fade_level = 11094;
		effectId[2] = SDL_HapticNewEffect(haptic, &effect[2]);
		if(effectId[2] < 0) printf("Error creating effect 2 \n\t%s\n", SDL_GetError());

		break;

	case 1:
		effect[0].type = SDL_HAPTIC_SINE;
		effect[0].periodic.length = 2000;
		effect[0].periodic.delay = 0;
		effect[0].periodic.button = 0;
		effect[0].periodic.interval = 0;
		effect[0].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[0].periodic.direction.dir[0] = 22300;
		effect[0].periodic.period = 500;
		effect[0].periodic.magnitude = 22962;
		effect[0].periodic.offset = 0;
		effect[0].periodic.phase = 0;
		effectId[0] = SDL_HapticNewEffect(haptic, &effect[0]);
		if(effectId[0] < 0) printf("Error creating effect 0 \n\t%s\n", SDL_GetError());

		effect[1].type = SDL_HAPTIC_SQUARE; /* if supported by SDL */
		effect[1].periodic.length = 2000;
		effect[1].periodic.delay = 0;
		effect[1].periodic.button = 0;
		effect[1].periodic.interval = 0;
		effect[1].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[1].periodic.direction.dir[0] = 3100;
		effect[1].periodic.period = 100;
		effect[1].periodic.magnitude = 15480;
		effect[1].periodic.offset = 0;
		effect[1].periodic.phase = 0;
		effect[1].periodic.attack_length = 325;
		effect[1].periodic.attack_level = 22962;
		effect[1].periodic.fade_length = 534;
		effect[1].periodic.fade_level = 0;
		effectId[1] = SDL_HapticNewEffect(haptic, &effect[1]);
		if(effectId[1] < 0) printf("Error creating effect 1 \n\t%s\n", SDL_GetError());

		effect[2].type = SDL_HAPTIC_SQUARE; /* if supported by SDL */
		effect[2].periodic.length = 500;
		effect[2].periodic.delay = 0;
		effect[2].periodic.button = 0;
		effect[2].periodic.interval = 0;
		effect[2].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[2].periodic.direction.dir[0] = 3100;
		effect[2].periodic.period = 500;
		effect[2].periodic.magnitude = 32767;
		effect[2].periodic.offset = 0;
		effect[2].periodic.phase = 0;
		effectId[2] = SDL_HapticNewEffect(haptic, &effect[2]);
		if(effectId[2] < 0) printf("Error creating effect 2 \n\t%s\n", SDL_GetError());

		break;

	case 2:
		effect[0].type = SDL_HAPTIC_SQUARE; /* if supported by SDL */
		effect[0].periodic.length = 3691;
		effect[0].periodic.delay = 0;
		effect[0].periodic.button = 0;
		effect[0].periodic.interval = 0;
		effect[0].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[0].periodic.direction.dir[0] = 6300;
		effect[0].periodic.period = 30;
		effect[0].periodic.magnitude = 13158;
		effect[0].periodic.offset = 0;
		effect[0].periodic.phase = 0;
		effect[0].periodic.attack_length = 718;
		effect[0].periodic.attack_level = 0;
		effect[0].periodic.fade_length = 1184;
		effect[0].periodic.fade_level = 0;
		effectId[0] = SDL_HapticNewEffect(haptic, &effect[0]);
		if(effectId[0] < 0) printf("Error creating effect 0 \n\t%s\n", SDL_GetError());

		effect[1].type = SDL_HAPTIC_SQUARE; /* if supported by SDL */
		effect[1].periodic.length = 1789;
		effect[1].periodic.delay = 718;
		effect[1].periodic.button = 0;
		effect[1].periodic.interval = 0;
		effect[1].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[1].periodic.direction.dir[0] = 0;
		effect[1].periodic.period = 60;
		effect[1].periodic.magnitude = 32767;
		effect[1].periodic.offset = 0;
		effect[1].periodic.phase = 0;
		effectId[1] = SDL_HapticNewEffect(haptic, &effect[1]);
		if(effectId[1] < 0) printf("Error creating effect 1 \n\t%s\n", SDL_GetError());

		break;

	case 3:
		effect[0].type = SDL_HAPTIC_SQUARE; /* if supported by SDL */
		effect[0].periodic.length = 2000;
		effect[0].periodic.delay = 0;
		effect[0].periodic.button = 0;
		effect[0].periodic.interval = 0;
		effect[0].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[0].periodic.direction.dir[0] = 0;
		effect[0].periodic.period = 100;
		effect[0].periodic.magnitude = 32767;
		effect[0].periodic.offset = 0;
		effect[0].periodic.phase = 0;
		effect[0].periodic.attack_length = 1000;
		effect[0].periodic.attack_level = 0;
		effect[0].periodic.fade_length = 1000;
		effect[0].periodic.fade_level = 0;
		effectId[0] = SDL_HapticNewEffect(haptic, &effect[0]);
		if(effectId[0] < 0) printf("Error creating effect 0 \n\t%s\n", SDL_GetError());

		effect[1].type = SDL_HAPTIC_SINE;
		effect[1].periodic.length = 6070;
		effect[1].periodic.delay = 0;
		effect[1].periodic.button = 0;
		effect[1].periodic.interval = 0;
		effect[1].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[1].periodic.direction.dir[0] = 6300;
		effect[1].periodic.period = 50;
		effect[1].periodic.magnitude = 16254;
		effect[1].periodic.offset = 0;
		effect[1].periodic.phase = 0;
		effect[1].periodic.attack_length = 128;
		effect[1].periodic.attack_level = 0;
		effect[1].periodic.fade_length = 128;
		effect[1].periodic.fade_level = 0;
		effectId[1] = SDL_HapticNewEffect(haptic, &effect[1]);
		if(effectId[1] < 0) printf("Error creating effect 1 \n\t%s\n", SDL_GetError());

		effect[2].type = SDL_HAPTIC_SQUARE; /* if supported by SDL */
		effect[2].periodic.length = 4070;
		effect[2].periodic.delay = 2000;
		effect[2].periodic.button = 0;
		effect[2].periodic.interval = 0;
		effect[2].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[2].periodic.direction.dir[0] = 0;
		effect[2].periodic.period = 50;
		effect[2].periodic.magnitude = 32767;
		effect[2].periodic.offset = 0;
		effect[2].periodic.phase = 0;
		effect[2].periodic.attack_length = 256;
		effect[2].periodic.attack_level = 0;
		effect[2].periodic.fade_length = 256;
		effect[2].periodic.fade_level = 0;
		effectId[2] = SDL_HapticNewEffect(haptic, &effect[2]);
		if(effectId[2] < 0) printf("Error creating effect 2 \n\t%s\n", SDL_GetError());

		break;

	case 4:
		effect[0].type = SDL_HAPTIC_TRIANGLE;
		effect[0].periodic.length = 4252;
		effect[0].periodic.delay = 0;
		effect[0].periodic.button = 0;
		effect[0].periodic.interval = 0;
		effect[0].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[0].periodic.direction.dir[0] = 6300;
		effect[0].periodic.period = 1103;
		effect[0].periodic.magnitude = 32767;
		effect[0].periodic.offset = 0;
		effect[0].periodic.phase = 0;
		effectId[0] = SDL_HapticNewEffect(haptic, &effect[0]);
		if(effectId[0] < 0) printf("Error creating effect 0 \n\t%s\n", SDL_GetError());

		effect[1].type = SDL_HAPTIC_SINE;
		effect[1].periodic.length = 438;
		effect[1].periodic.delay = 283;
		effect[1].periodic.button = 0;
		effect[1].periodic.interval = 0;
		effect[1].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[1].periodic.direction.dir[0] = 0;
		effect[1].periodic.period = 69;
		effect[1].periodic.magnitude = 20124;
		effect[1].periodic.offset = 0;
		effect[1].periodic.phase = 0;
		effect[1].periodic.attack_length = 59;
		effect[1].periodic.attack_level = 12384;
		effect[1].periodic.fade_length = 379;
		effect[1].periodic.fade_level = 0;
		effectId[1] = SDL_HapticNewEffect(haptic, &effect[1]);
		if(effectId[1] < 0) printf("Error creating effect 1 \n\t%s\n", SDL_GetError());

		effect[2].type = SDL_HAPTIC_SINE;
		effect[2].periodic.length = 2183;
		effect[2].periodic.delay = 2094;
		effect[2].periodic.button = 0;
		effect[2].periodic.interval = 0;
		effect[2].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[2].periodic.direction.dir[0] = 6500;
		effect[2].periodic.period = 202;
		effect[2].periodic.magnitude = 32767;
		effect[2].periodic.offset = 0;
		effect[2].periodic.phase = 0;
		effect[2].periodic.attack_length = 1553;
		effect[2].periodic.attack_level = 0;
		effect[2].periodic.fade_length = 383;
		effect[2].periodic.fade_level = 0;
		effectId[2] = SDL_HapticNewEffect(haptic, &effect[2]);
		if(effectId[2] < 0) printf("Error creating effect 2 \n\t%s\n", SDL_GetError());

		effect[3].type = SDL_HAPTIC_SINE;
		effect[3].periodic.length = 731;
		effect[3].periodic.delay = 824;
		effect[3].periodic.button = 0;
		effect[3].periodic.interval = 0;
		effect[3].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[3].periodic.direction.dir[0] = 0;
		effect[3].periodic.period = 69;
		effect[3].periodic.magnitude = 20640;
		effect[3].periodic.offset = 0;
		effect[3].periodic.phase = 0;
		effect[3].periodic.attack_length = 0;
		effect[3].periodic.attack_level = 20640;
		effect[3].periodic.fade_length = 731;
		effect[3].periodic.fade_level = 0;
		effectId[3] = SDL_HapticNewEffect(haptic, &effect[3]);
		if(effectId[3] < 0) printf("Error creating effect 3 \n\t%s\n", SDL_GetError());

		break;

	case 5:
		effect[0].type = SDL_HAPTIC_SQUARE; /* if supported by SDL */
		effect[0].periodic.length = 6177;
		effect[0].periodic.delay = 0;
		effect[0].periodic.button = 0;
		effect[0].periodic.interval = 0;
		effect[0].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[0].periodic.direction.dir[0] = 0;
		effect[0].periodic.period = 55;
		effect[0].periodic.magnitude = 9804;
		effect[0].periodic.offset = 0;
		effect[0].periodic.phase = 0;
		effect[0].periodic.attack_length = 76;
		effect[0].periodic.attack_level = 0;
		effect[0].periodic.fade_length = 500;
		effect[0].periodic.fade_level = 0;
		effectId[0] = SDL_HapticNewEffect(haptic, &effect[0]);
		if(effectId[0] < 0) printf("Error creating effect 0 \n\t%s\n", SDL_GetError());

		effect[1].type = SDL_HAPTIC_SQUARE; /* if supported by SDL */
		effect[1].periodic.length = 1023;
		effect[1].periodic.delay = 350;
		effect[1].periodic.button = 0;
		effect[1].periodic.interval = 0;
		effect[1].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[1].periodic.direction.dir[0] = 0;
		effect[1].periodic.period = 210;
		effect[1].periodic.magnitude = 32767;
		effect[1].periodic.offset = 0;
		effect[1].periodic.phase = 0;
		effectId[1] = SDL_HapticNewEffect(haptic, &effect[1]);
		if(effectId[1] < 0) printf("Error creating effect 1 \n\t%s\n", SDL_GetError());

		effect[2].type = SDL_HAPTIC_SQUARE; /* if supported by SDL */
		effect[2].periodic.length = 199;
		effect[2].periodic.delay = 1600;
		effect[2].periodic.button = 0;
		effect[2].periodic.interval = 0;
		effect[2].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[2].periodic.direction.dir[0] = 0;
		effect[2].periodic.period = 202;
		effect[2].periodic.magnitude = 32767;
		effect[2].periodic.offset = 0;
		effect[2].periodic.phase = 0;
		effectId[2] = SDL_HapticNewEffect(haptic, &effect[2]);
		if(effectId[2] < 0) printf("Error creating effect 2 \n\t%s\n", SDL_GetError());

		effect[3].type = SDL_HAPTIC_SQUARE; /* if supported by SDL */
		effect[3].periodic.length = 500;
		effect[3].periodic.delay = 1750;
		effect[3].periodic.button = 0;
		effect[3].periodic.interval = 0;
		effect[3].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[3].periodic.direction.dir[0] = 0;
		effect[3].periodic.period = 59;
		effect[3].periodic.magnitude = 32767;
		effect[3].periodic.offset = 0;
		effect[3].periodic.phase = 0;
		effect[3].periodic.attack_length = 0;
		effect[3].periodic.attack_level = 32767;
		effect[3].periodic.fade_length = 500;
		effect[3].periodic.fade_level = 0;
		effectId[3] = SDL_HapticNewEffect(haptic, &effect[3]);
		if(effectId[3] < 0) printf("Error creating effect 3 \n\t%s\n", SDL_GetError());

		effect[4].type = SDL_HAPTIC_SQUARE; /* if supported by SDL */
		effect[4].periodic.length = 1050;
		effect[4].periodic.delay = 2850;
		effect[4].periodic.button = 0;
		effect[4].periodic.interval = 0;
		effect[4].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[4].periodic.direction.dir[0] = 0;
		effect[4].periodic.period = 210;
		effect[4].periodic.magnitude = 32767;
		effect[4].periodic.offset = 0;
		effect[4].periodic.phase = 0;
		effectId[4] = SDL_HapticNewEffect(haptic, &effect[4]);
		if(effectId[4] < 0) printf("Error creating effect 4 \n\t%s\n", SDL_GetError());

		effect[5].type = SDL_HAPTIC_SQUARE; /* if supported by SDL */
		effect[5].periodic.length = 1049;
		effect[5].periodic.delay = 4400;
		effect[5].periodic.button = 0;
		effect[5].periodic.interval = 0;
		effect[5].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[5].periodic.direction.dir[0] = 0;
		effect[5].periodic.period = 210;
		effect[5].periodic.magnitude = 32767;
		effect[5].periodic.offset = 0;
		effect[5].periodic.phase = 0;
		effectId[5] = SDL_HapticNewEffect(haptic, &effect[5]);
		if(effectId[5] < 0) printf("Error creating effect 5 \n\t%s\n", SDL_GetError());

		break;

	case 6:
		effect[0].type = SDL_HAPTIC_SQUARE; /* if supported by SDL */
		effect[0].periodic.length = 2000;
		effect[0].periodic.delay = 0;
		effect[0].periodic.button = 0;
		effect[0].periodic.interval = 0;
		effect[0].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[0].periodic.direction.dir[0] = 6300;
		effect[0].periodic.period = 295;
		effect[0].periodic.magnitude = 22704;
		effect[0].periodic.offset = 0;
		effect[0].periodic.phase = 0;
		effect[0].periodic.attack_length = 390;
		effect[0].periodic.attack_level = 32767;
		effect[0].periodic.fade_length = 967;
		effect[0].periodic.fade_level = 0;
		effectId[0] = SDL_HapticNewEffect(haptic, &effect[0]);
		if(effectId[0] < 0) printf("Error creating effect 0 \n\t%s\n", SDL_GetError());

		effect[1].type = SDL_HAPTIC_SINE;
		effect[1].periodic.length = 3500;
		effect[1].periodic.delay = 0;
		effect[1].periodic.button = 0;
		effect[1].periodic.interval = 0;
		effect[1].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[1].periodic.direction.dir[0] = 0;
		effect[1].periodic.period = 100;
		effect[1].periodic.magnitude = 32250;
		effect[1].periodic.offset = 0;
		effect[1].periodic.phase = 0;
		effect[1].periodic.attack_length = 2572;
		effect[1].periodic.attack_level = 0;
		effect[1].periodic.fade_length = 0;
		effect[1].periodic.fade_level = 32250;
		effectId[1] = SDL_HapticNewEffect(haptic, &effect[1]);
		if(effectId[1] < 0) printf("Error creating effect 1 \n\t%s\n", SDL_GetError());

		break;

	case 7:
		effect[0].type = SDL_HAPTIC_SAWTOOTHUP;
		effect[0].periodic.length = 1125;
		effect[0].periodic.delay = 3402;
		effect[0].periodic.button = 0;
		effect[0].periodic.interval = 0;
		effect[0].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[0].periodic.direction.dir[0] = 0;
		effect[0].periodic.period = 6;
		effect[0].periodic.magnitude = 32767;
		effect[0].periodic.offset = 0;
		effect[0].periodic.phase = 0;
		effect[0].periodic.attack_length = 256;
		effect[0].periodic.attack_level = 0;
		effect[0].periodic.fade_length = 644;
		effect[0].periodic.fade_level = 0;
		effectId[0] = SDL_HapticNewEffect(haptic, &effect[0]);
		if(effectId[0] < 0) printf("Error creating effect 0 \n\t%s\n", SDL_GetError());

		effect[1].type = SDL_HAPTIC_SQUARE; /* if supported by SDL */
		effect[1].periodic.length = 104;
		effect[1].periodic.delay = 3273;
		effect[1].periodic.button = 0;
		effect[1].periodic.interval = 0;
		effect[1].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[1].periodic.direction.dir[0] = 19200;
		effect[1].periodic.period = 97;
		effect[1].periodic.magnitude = 32767;
		effect[1].periodic.offset = 0;
		effect[1].periodic.phase = 0;
		effectId[1] = SDL_HapticNewEffect(haptic, &effect[1]);
		if(effectId[1] < 0) printf("Error creating effect 1 \n\t%s\n", SDL_GetError());

		effect[2].type = SDL_HAPTIC_SINE;
		effect[2].periodic.length = 397;
		effect[2].periodic.delay = 1233;
		effect[2].periodic.button = 0;
		effect[2].periodic.interval = 0;
		effect[2].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[2].periodic.direction.dir[0] = 0;
		effect[2].periodic.period = 24;
		effect[2].periodic.magnitude = 32767;
		effect[2].periodic.offset = 0;
		effect[2].periodic.phase = 0;
		effect[2].periodic.attack_length = 0;
		effect[2].periodic.attack_level = 32767;
		effect[2].periodic.fade_length = 330;
		effect[2].periodic.fade_level = 0;
		effectId[2] = SDL_HapticNewEffect(haptic, &effect[2]);
		if(effectId[2] < 0) printf("Error creating effect 2 \n\t%s\n", SDL_GetError());

		effect[3].type = SDL_HAPTIC_SINE;
		effect[3].periodic.length = 397;
		effect[3].periodic.delay = 1921;
		effect[3].periodic.button = 0;
		effect[3].periodic.interval = 0;
		effect[3].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[3].periodic.direction.dir[0] = 0;
		effect[3].periodic.period = 24;
		effect[3].periodic.magnitude = 32767;
		effect[3].periodic.offset = 0;
		effect[3].periodic.phase = 0;
		effect[3].periodic.attack_length = 0;
		effect[3].periodic.attack_level = 32767;
		effect[3].periodic.fade_length = 330;
		effect[3].periodic.fade_level = 0;
		effectId[3] = SDL_HapticNewEffect(haptic, &effect[3]);
		if(effectId[3] < 0) printf("Error creating effect 3 \n\t%s\n", SDL_GetError());

		effect[4].type = SDL_HAPTIC_SINE;
		effect[4].periodic.length = 397;
		effect[4].periodic.delay = 2623;
		effect[4].periodic.button = 0;
		effect[4].periodic.interval = 0;
		effect[4].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[4].periodic.direction.dir[0] = 0;
		effect[4].periodic.period = 24;
		effect[4].periodic.magnitude = 32767;
		effect[4].periodic.offset = 0;
		effect[4].periodic.phase = 0;
		effect[4].periodic.attack_length = 0;
		effect[4].periodic.attack_level = 32767;
		effect[4].periodic.fade_length = 330;
		effect[4].periodic.fade_level = 0;
		effectId[4] = SDL_HapticNewEffect(haptic, &effect[4]);
		if(effectId[4] < 0) printf("Error creating effect 4 \n\t%s\n", SDL_GetError());

		effect[5].type = SDL_HAPTIC_SINE;
		effect[5].periodic.length = 99;
		effect[5].periodic.delay = 399;
		effect[5].periodic.button = 0;
		effect[5].periodic.interval = 0;
		effect[5].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[5].periodic.direction.dir[0] = 12800;
		effect[5].periodic.period = 100;
		effect[5].periodic.magnitude = 32767;
		effect[5].periodic.offset = 0;
		effect[5].periodic.phase = 0;
		effectId[5] = SDL_HapticNewEffect(haptic, &effect[5]);
		if(effectId[5] < 0) printf("Error creating effect 5 \n\t%s\n", SDL_GetError());

		effect[6].type = SDL_HAPTIC_SINE;
		effect[6].periodic.length = 99;
		effect[6].periodic.delay = 1103;
		effect[6].periodic.button = 0;
		effect[6].periodic.interval = 0;
		effect[6].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[6].periodic.direction.dir[0] = 12600;
		effect[6].periodic.period = 100;
		effect[6].periodic.magnitude = 32767;
		effect[6].periodic.offset = 0;
		effect[6].periodic.phase = 0;
		effectId[6] = SDL_HapticNewEffect(haptic, &effect[6]);
		if(effectId[6] < 0) printf("Error creating effect 6 \n\t%s\n", SDL_GetError());

		effect[7].type = SDL_HAPTIC_SINE;
		effect[7].periodic.length = 99;
		effect[7].periodic.delay = 1812;
		effect[7].periodic.button = 0;
		effect[7].periodic.interval = 0;
		effect[7].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[7].periodic.direction.dir[0] = 12700;
		effect[7].periodic.period = 100;
		effect[7].periodic.magnitude = 32767;
		effect[7].periodic.offset = 0;
		effect[7].periodic.phase = 0;
		effectId[7] = SDL_HapticNewEffect(haptic, &effect[7]);
		if(effectId[7] < 0) printf("Error creating effect 7 \n\t%s\n", SDL_GetError());

		effect[8].type = SDL_HAPTIC_SINE;
		effect[8].periodic.length = 99;
		effect[8].periodic.delay = 2504;
		effect[8].periodic.button = 0;
		effect[8].periodic.interval = 0;
		effect[8].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[8].periodic.direction.dir[0] = 12700;
		effect[8].periodic.period = 100;
		effect[8].periodic.magnitude = 32767;
		effect[8].periodic.offset = 0;
		effect[8].periodic.phase = 0;
		effectId[8] = SDL_HapticNewEffect(haptic, &effect[8]);
		if(effectId[8] < 0) printf("Error creating effect 8 \n\t%s\n", SDL_GetError());

		effect[9].type = SDL_HAPTIC_SINE;
		effect[9].periodic.length = 360;
		effect[9].periodic.delay = 515;
		effect[9].periodic.button = 0;
		effect[9].periodic.interval = 0;
		effect[9].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[9].periodic.direction.dir[0] = 0;
		effect[9].periodic.period = 25;
		effect[9].periodic.magnitude = 32767;
		effect[9].periodic.offset = 0;
		effect[9].periodic.phase = 0;
		effect[9].periodic.attack_length = 0;
		effect[9].periodic.attack_level = 32767;
		effect[9].periodic.fade_length = 303;
		effect[9].periodic.fade_level = 0;
		effectId[9] = SDL_HapticNewEffect(haptic, &effect[9]);
		if(effectId[9] < 0) printf("Error creating effect 9 \n\t%s\n", SDL_GetError());

		break;

	case 8:
		effect[0].type = SDL_HAPTIC_SINE;
		effect[0].periodic.length = 4000;
		effect[0].periodic.delay = 0;
		effect[0].periodic.button = 0;
		effect[0].periodic.interval = 0;
		effect[0].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[0].periodic.direction.dir[0] = 6300;
		effect[0].periodic.period = 2000;
		effect[0].periodic.magnitude = 29670;
		effect[0].periodic.offset = 0;
		effect[0].periodic.phase = 0;
		effectId[0] = SDL_HapticNewEffect(haptic, &effect[0]);
		if(effectId[0] < 0) printf("Error creating effect 0 \n\t%s\n", SDL_GetError());

		effect[1].type = SDL_HAPTIC_SQUARE; /* if supported by SDL */
		effect[1].periodic.length = 400;
		effect[1].periodic.delay = 600;
		effect[1].periodic.button = 0;
		effect[1].periodic.interval = 0;
		effect[1].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[1].periodic.direction.dir[0] = 1400;
		effect[1].periodic.period = 20;
		effect[1].periodic.magnitude = 32767;
		effect[1].periodic.offset = 0;
		effect[1].periodic.phase = 0;
		effect[1].periodic.attack_length = 0;
		effect[1].periodic.attack_level = 32767;
		effect[1].periodic.fade_length = 400;
		effect[1].periodic.fade_level = 0;
		effectId[1] = SDL_HapticNewEffect(haptic, &effect[1]);
		if(effectId[1] < 0) printf("Error creating effect 1 \n\t%s\n", SDL_GetError());

		effect[2].type = SDL_HAPTIC_SQUARE; /* if supported by SDL */
		effect[2].periodic.length = 400;
		effect[2].periodic.delay = 1600;
		effect[2].periodic.button = 0;
		effect[2].periodic.interval = 0;
		effect[2].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[2].periodic.direction.dir[0] = 24100;
		effect[2].periodic.period = 20;
		effect[2].periodic.magnitude = 32767;
		effect[2].periodic.offset = 0;
		effect[2].periodic.phase = 0;
		effect[2].periodic.attack_length = 0;
		effect[2].periodic.attack_level = 32767;
		effect[2].periodic.fade_length = 400;
		effect[2].periodic.fade_level = 0;
		effectId[2] = SDL_HapticNewEffect(haptic, &effect[2]);
		if(effectId[2] < 0) printf("Error creating effect 2 \n\t%s\n", SDL_GetError());

		effect[3].type = SDL_HAPTIC_SQUARE; /* if supported by SDL */
		effect[3].periodic.length = 400;
		effect[3].periodic.delay = 2600;
		effect[3].periodic.button = 0;
		effect[3].periodic.interval = 0;
		effect[3].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[3].periodic.direction.dir[0] = 1400;
		effect[3].periodic.period = 20;
		effect[3].periodic.magnitude = 32767;
		effect[3].periodic.offset = 0;
		effect[3].periodic.phase = 0;
		effect[3].periodic.attack_length = 0;
		effect[3].periodic.attack_level = 32767;
		effect[3].periodic.fade_length = 400;
		effect[3].periodic.fade_level = 0;
		effectId[3] = SDL_HapticNewEffect(haptic, &effect[3]);
		if(effectId[3] < 0) printf("Error creating effect 3 \n\t%s\n", SDL_GetError());

		effect[4].type = SDL_HAPTIC_SQUARE; /* if supported by SDL */
		effect[4].periodic.length = 400;
		effect[4].periodic.delay = 3600;
		effect[4].periodic.button = 0;
		effect[4].periodic.interval = 0;
		effect[4].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[4].periodic.direction.dir[0] = 24100;
		effect[4].periodic.period = 20;
		effect[4].periodic.magnitude = 32767;
		effect[4].periodic.offset = 0;
		effect[4].periodic.phase = 0;
		effect[4].periodic.attack_length = 0;
		effect[4].periodic.attack_level = 32767;
		effect[4].periodic.fade_length = 400;
		effect[4].periodic.fade_level = 0;
		effectId[4] = SDL_HapticNewEffect(haptic, &effect[4]);
		if(effectId[4] < 0) printf("Error creating effect 4 \n\t%s\n", SDL_GetError());

		effect[5].type = SDL_HAPTIC_SQUARE; /* if supported by SDL */
		effect[5].periodic.length = 3037;
		effect[5].periodic.delay = 4000;
		effect[5].periodic.button = 0;
		effect[5].periodic.interval = 0;
		effect[5].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[5].periodic.direction.dir[0] = 6300;
		effect[5].periodic.period = 20;
		effect[5].periodic.magnitude = 32767;
		effect[5].periodic.offset = 0;
		effect[5].periodic.phase = 0;
		effect[5].periodic.attack_length = 800;
		effect[5].periodic.attack_level = 0;
		effect[5].periodic.fade_length = 256;
		effect[5].periodic.fade_level = 0;
		effectId[5] = SDL_HapticNewEffect(haptic, &effect[5]);
		if(effectId[5] < 0) printf("Error creating effect 5 \n\t%s\n", SDL_GetError());

		effect[6].type = SDL_HAPTIC_SQUARE; /* if supported by SDL */
		effect[6].periodic.length = 99;
		effect[6].periodic.delay = 500;
		effect[6].periodic.button = 0;
		effect[6].periodic.interval = 0;
		effect[6].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[6].periodic.direction.dir[0] = 0;
		effect[6].periodic.period = 100;
		effect[6].periodic.magnitude = 32767;
		effect[6].periodic.offset = 0;
		effect[6].periodic.phase = 0;
		effectId[6] = SDL_HapticNewEffect(haptic, &effect[6]);
		if(effectId[6] < 0) printf("Error creating effect 6 \n\t%s\n", SDL_GetError());

		effect[7].type = SDL_HAPTIC_SQUARE; /* if supported by SDL */
		effect[7].periodic.length = 100;
		effect[7].periodic.delay = 1500;
		effect[7].periodic.button = 0;
		effect[7].periodic.interval = 0;
		effect[7].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[7].periodic.direction.dir[0] = 0;
		effect[7].periodic.period = 100;
		effect[7].periodic.magnitude = 32767;
		effect[7].periodic.offset = 0;
		effect[7].periodic.phase = 0;
		effectId[7] = SDL_HapticNewEffect(haptic, &effect[7]);
		if(effectId[7] < 0) printf("Error creating effect 7 \n\t%s\n", SDL_GetError());

		effect[8].type = SDL_HAPTIC_SQUARE; /* if supported by SDL */
		effect[8].periodic.length = 100;
		effect[8].periodic.delay = 2500;
		effect[8].periodic.button = 0;
		effect[8].periodic.interval = 0;
		effect[8].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[8].periodic.direction.dir[0] = 0;
		effect[8].periodic.period = 100;
		effect[8].periodic.magnitude = 32767;
		effect[8].periodic.offset = 0;
		effect[8].periodic.phase = 0;
		effectId[8] = SDL_HapticNewEffect(haptic, &effect[8]);
		if(effectId[8] < 0) printf("Error creating effect 8 \n\t%s\n", SDL_GetError());

		effect[9].type = SDL_HAPTIC_SQUARE; /* if supported by SDL */
		effect[9].periodic.length = 100;
		effect[9].periodic.delay = 3500;
		effect[9].periodic.button = 0;
		effect[9].periodic.interval = 0;
		effect[9].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[9].periodic.direction.dir[0] = 0;
		effect[9].periodic.period = 100;
		effect[9].periodic.magnitude = 32767;
		effect[9].periodic.offset = 0;
		effect[9].periodic.phase = 0;
		effectId[9] = SDL_HapticNewEffect(haptic, &effect[9]);
		if(effectId[9] < 0) printf("Error creating effect 9 \n\t%s\n", SDL_GetError());

		break;

	case 9:
		effect[0].type = SDL_HAPTIC_SINE;
		effect[0].periodic.length = 1108;
		effect[0].periodic.delay = 180;
		effect[0].periodic.button = 0;
		effect[0].periodic.interval = 0;
		effect[0].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[0].periodic.direction.dir[0] = 0;
		effect[0].periodic.period = 22;
		effect[0].periodic.magnitude = 15480;
		effect[0].periodic.offset = 10836;
		effect[0].periodic.phase = 0;
		effect[0].periodic.attack_length = 221;
		effect[0].periodic.attack_level = 21930;
		effect[0].periodic.fade_length = 886;
		effect[0].periodic.fade_level = 4128;
		effectId[0] = SDL_HapticNewEffect(haptic, &effect[0]);
		if(effectId[0] < 0) printf("Error creating effect 0 \n\t%s\n", SDL_GetError());

		effect[1].type = SDL_HAPTIC_CONSTANT;
		effect[1].constant.length = 77;
		effect[1].constant.delay = 0;
		effect[1].constant.button = 0;
		effect[1].constant.interval = 0;
		effect[1].constant.direction.type = SDL_HAPTIC_POLAR;
		effect[1].constant.direction.dir[0] = 12800;
		effect[1].constant.level = 32767;
		effectId[1] = SDL_HapticNewEffect(haptic, &effect[1]);
		if(effectId[1] < 0) printf("Error creating effect 1 \n\t%s\n", SDL_GetError());

		effect[2].type = SDL_HAPTIC_SINE;
		effect[2].periodic.length = 397;
		effect[2].periodic.delay = 25;
		effect[2].periodic.button = 0;
		effect[2].periodic.interval = 0;
		effect[2].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[2].periodic.direction.dir[0] = 0;
		effect[2].periodic.period = 176;
		effect[2].periodic.magnitude = 14964;
		effect[2].periodic.offset = 0;
		effect[2].periodic.phase = 0;
		effect[2].periodic.attack_length = 117;
		effect[2].periodic.attack_level = 0;
		effect[2].periodic.fade_length = 264;
		effect[2].periodic.fade_level = 0;
		effectId[2] = SDL_HapticNewEffect(haptic, &effect[2]);
		if(effectId[2] < 0) printf("Error creating effect 2 \n\t%s\n", SDL_GetError());

		effect[3].type = SDL_HAPTIC_CONSTANT;
		effect[3].constant.length = 790;
		effect[3].constant.delay = 1077;
		effect[3].constant.button = 0;
		effect[3].constant.interval = 0;
		effect[3].constant.direction.type = SDL_HAPTIC_POLAR;
		effect[3].constant.direction.dir[0] = 6300;
		effect[3].constant.level = 32767;
		effect[3].constant.attack_length = 225;
		effect[3].constant.attack_level = 3870;
		effect[3].constant.fade_length = 564;
		effect[3].constant.fade_level = 0;
		effectId[3] = SDL_HapticNewEffect(haptic, &effect[3]);
		if(effectId[3] < 0) printf("Error creating effect 3 \n\t%s\n", SDL_GetError());

		effect[4].type = SDL_HAPTIC_CONSTANT;
		effect[4].constant.length = 860;
		effect[4].constant.delay = 2335;
		effect[4].constant.button = 0;
		effect[4].constant.interval = 0;
		effect[4].constant.direction.type = SDL_HAPTIC_POLAR;
		effect[4].constant.direction.dir[0] = 19000;
		effect[4].constant.level = 32767;
		effect[4].constant.attack_length = 235;
		effect[4].constant.attack_level = 2580;
		effect[4].constant.fade_length = 625;
		effect[4].constant.fade_level = 0;
		effectId[4] = SDL_HapticNewEffect(haptic, &effect[4]);
		if(effectId[4] < 0) printf("Error creating effect 4 \n\t%s\n", SDL_GetError());

		effect[5].type = SDL_HAPTIC_SINE;
		effect[5].periodic.length = 4381;
		effect[5].periodic.delay = 1314;
		effect[5].periodic.button = 0;
		effect[5].periodic.interval = 0;
		effect[5].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[5].periodic.direction.dir[0] = 0;
		effect[5].periodic.period = 22;
		effect[5].periodic.magnitude = 7224;
		effect[5].periodic.offset = 10836;
		effect[5].periodic.phase = 0;
		effectId[5] = SDL_HapticNewEffect(haptic, &effect[5]);
		if(effectId[5] < 0) printf("Error creating effect 5 \n\t%s\n", SDL_GetError());

		effect[6].type = SDL_HAPTIC_TRIANGLE;
		effect[6].periodic.length = 894;
		effect[6].periodic.delay = 4131;
		effect[6].periodic.button = 0;
		effect[6].periodic.interval = 0;
		effect[6].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[6].periodic.direction.dir[0] = 0;
		effect[6].periodic.period = 39;
		effect[6].periodic.magnitude = 13932;
		effect[6].periodic.offset = -258;
		effect[6].periodic.phase = 0;
		effect[6].periodic.attack_length = 894;
		effect[6].periodic.attack_level = 0;
		effect[6].periodic.fade_length = 0;
		effect[6].periodic.fade_level = 13932;
		effectId[6] = SDL_HapticNewEffect(haptic, &effect[6]);
		if(effectId[6] < 0) printf("Error creating effect 6 \n\t%s\n", SDL_GetError());

		effect[7].type = SDL_HAPTIC_SINE;
		effect[7].periodic.length = 1134;
		effect[7].periodic.delay = 3350;
		effect[7].periodic.button = 0;
		effect[7].periodic.interval = 0;
		effect[7].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[7].periodic.direction.dir[0] = 7800;
		effect[7].periodic.period = 2458;
		effect[7].periodic.magnitude = 20640;
		effect[7].periodic.offset = 0;
		effect[7].periodic.phase = 0;
		effectId[7] = SDL_HapticNewEffect(haptic, &effect[7]);
		if(effectId[7] < 0) printf("Error creating effect 7 \n\t%s\n", SDL_GetError());

		effect[8].type = SDL_HAPTIC_SINE;
		effect[8].periodic.length = 677;
		effect[8].periodic.delay = 5025;
		effect[8].periodic.button = 0;
		effect[8].periodic.interval = 0;
		effect[8].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[8].periodic.direction.dir[0] = 0;
		effect[8].periodic.period = 176;
		effect[8].periodic.magnitude = 16512;
		effect[8].periodic.offset = 0;
		effect[8].periodic.phase = 0;
		effect[8].periodic.attack_length = 199;
		effect[8].periodic.attack_level = 0;
		effect[8].periodic.fade_length = 0;
		effect[8].periodic.fade_level = 16512;
		effectId[8] = SDL_HapticNewEffect(haptic, &effect[8]);
		if(effectId[8] < 0) printf("Error creating effect 8 \n\t%s\n", SDL_GetError());

		effect[9].type = SDL_HAPTIC_SQUARE; /* if supported by SDL */
		effect[9].periodic.length = 154;
		effect[9].periodic.delay = 5695;
		effect[9].periodic.button = 0;
		effect[9].periodic.interval = 0;
		effect[9].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[9].periodic.direction.dir[0] = 0;
		effect[9].periodic.period = 216;
		effect[9].periodic.magnitude = 32767;
		effect[9].periodic.offset = 0;
		effect[9].periodic.phase = 0;
		effectId[9] = SDL_HapticNewEffect(haptic, &effect[9]);
		if(effectId[9] < 0) printf("Error creating effect 9 \n\t%s\n", SDL_GetError());

		break;

	case 10:
		effect[0].type = SDL_HAPTIC_SQUARE; /* if supported by SDL */
		effect[0].periodic.length = 2700;
		effect[0].periodic.delay = 0;
		effect[0].periodic.button = 0;
		effect[0].periodic.interval = 0;
		effect[0].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[0].periodic.direction.dir[0] = 0;
		effect[0].periodic.period = 3800;
		effect[0].periodic.magnitude = 19092;
		effect[0].periodic.offset = 0;
		effect[0].periodic.phase = 0;
		effect[0].periodic.attack_length = 893;
		effect[0].periodic.attack_level = 32767;
		effect[0].periodic.fade_length = 715;
		effect[0].periodic.fade_level = 27864;
		effectId[0] = SDL_HapticNewEffect(haptic, &effect[0]);
		if(effectId[0] < 0) printf("Error creating effect 0 \n\t%s\n", SDL_GetError());

		effect[1].type = SDL_HAPTIC_SQUARE; /* if supported by SDL */
		effect[1].periodic.length = 2800;
		effect[1].periodic.delay = 0;
		effect[1].periodic.button = 0;
		effect[1].periodic.interval = 0;
		effect[1].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[1].periodic.direction.dir[0] = 6300;
		effect[1].periodic.period = 400;
		effect[1].periodic.magnitude = 0;
		effect[1].periodic.offset = 0;
		effect[1].periodic.phase = 0;
		effect[1].periodic.attack_length = 0;
		effect[1].periodic.attack_level = 0;
		effect[1].periodic.fade_length = 1992;
		effect[1].periodic.fade_level = 32250;
		effectId[1] = SDL_HapticNewEffect(haptic, &effect[1]);
		if(effectId[1] < 0) printf("Error creating effect 1 \n\t%s\n", SDL_GetError());

		effect[2].type = SDL_HAPTIC_TRIANGLE;
		effect[2].periodic.length = 1500;
		effect[2].periodic.delay = 0;
		effect[2].periodic.button = 0;
		effect[2].periodic.interval = 0;
		effect[2].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[2].periodic.direction.dir[0] = 6300;
		effect[2].periodic.period = 1500;
		effect[2].periodic.magnitude = 27864;
		effect[2].periodic.offset = 0;
		effect[2].periodic.phase = 0;
		effectId[2] = SDL_HapticNewEffect(haptic, &effect[2]);
		if(effectId[2] < 0) printf("Error creating effect 2 \n\t%s\n", SDL_GetError());

		break;

	case 11:
		effect[0].type = SDL_HAPTIC_SQUARE; /* if supported by SDL */
		effect[0].periodic.length = 2597;
		effect[0].periodic.delay = 0;
		effect[0].periodic.button = 0;
		effect[0].periodic.interval = 0;
		effect[0].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[0].periodic.direction.dir[0] = 22400;
		effect[0].periodic.period = 111;
		effect[0].periodic.magnitude = 14706;
		effect[0].periodic.offset = 0;
		effect[0].periodic.phase = 0;
		effectId[0] = SDL_HapticNewEffect(haptic, &effect[0]);
		if(effectId[0] < 0) printf("Error creating effect 0 \n\t%s\n", SDL_GetError());

		effect[1].type = SDL_HAPTIC_SINE;
		effect[1].periodic.length = 2597;
		effect[1].periodic.delay = 0;
		effect[1].periodic.button = 0;
		effect[1].periodic.interval = 0;
		effect[1].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[1].periodic.direction.dir[0] = 19100;
		effect[1].periodic.period = 2000;
		effect[1].periodic.magnitude = 29670;
		effect[1].periodic.offset = 0;
		effect[1].periodic.phase = 0;
		effectId[1] = SDL_HapticNewEffect(haptic, &effect[1]);
		if(effectId[1] < 0) printf("Error creating effect 1 \n\t%s\n", SDL_GetError());

		effect[2].type = SDL_HAPTIC_SINE;
		effect[2].periodic.length = 2597;
		effect[2].periodic.delay = 0;
		effect[2].periodic.button = 0;
		effect[2].periodic.interval = 0;
		effect[2].periodic.direction.type = SDL_HAPTIC_POLAR;
		effect[2].periodic.direction.dir[0] = 0;
		effect[2].periodic.period = 2000;
		effect[2].periodic.magnitude = 29670;
		effect[2].periodic.offset = 0;
		effect[2].periodic.phase = 0;
		effectId[2] = SDL_HapticNewEffect(haptic, &effect[2]);
		if(effectId[2] < 0) printf("Error creating effect 2 \n\t%s\n", SDL_GetError());

		break;

	default:
		break;
	};
}
