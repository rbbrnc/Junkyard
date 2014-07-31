/*
 * Audio Volume Applet -- Mixer interface
 */

#ifndef VOLUME_MIXER_H
#define VOLUME_MIXER_H

#ifdef __cplusplus
extern "C" {
#endif


int mixer_open(void);
int mixer_close(void);
int mixer_set_volume(const int value);
int mixer_get_volume(void);

#ifdef __cplusplus
};
#endif

#endif /* VOLUME_MIXER_H */
