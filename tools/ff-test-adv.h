#ifndef __FF_TEST_ADV_H__
#define __FF_TEST_ADV_H__

#define BUILT_EFFECTS   12


#define NAMELEN     30
#define AXES        3   // Maximum axes supported
#define EFFECTS     10	// Maximum simultaneous effects

typedef struct __hapticdevice {
    SDL_Haptic *device;
    char name[NAMELEN+1];          // Name
    unsigned int num;       // Num of this device
    unsigned int supported; // Capabilities
    unsigned int axes;      // Count of axes
    unsigned int numEffects, numEffectsPlaying;
    bool open;

} hapticDevice;


struct effect_req_cap {
    int neff;
    int mask;
};

#endif
