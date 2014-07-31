/*
 * mixer_alsa.c
 *
 * Audio Volume Applet -- ALSA mixer interface
 */

#include <stdio.h>
#include <alsa/asoundlib.h>
#include "mixer.h"

static snd_mixer_t *handle = NULL;
static snd_mixer_elem_t *elem = NULL;

int mixer_open(void)
{
	int rc;
	snd_mixer_selem_id_t *sid;
	const char *card = "default";
	const char *selem_name = "Master";

	if (handle) {
		fprintf(stderr, "%s: Mixer already open\n", __func__);
		return 1;
	}

	rc = snd_mixer_open(&handle, 0);
	if (rc < 0) {
		goto error;
	}

	rc = snd_mixer_attach(handle, card);
	if (rc < 0) {
		goto error;
	}

	rc = snd_mixer_selem_register(handle, NULL, NULL);
	if (rc < 0) {
		goto error;
	}

	rc = snd_mixer_load(handle);
	if (rc < 0) {
		goto error;
	}

	snd_mixer_selem_id_alloca(&sid);
	snd_mixer_selem_id_set_index(sid, 0);
	snd_mixer_selem_id_set_name(sid, selem_name);
	elem = snd_mixer_find_selem(handle, sid);

	return 0;

error:
	fprintf(stderr, "%s: ERROR: %s\n", __func__, snd_strerror(rc));
	mixer_close();
	return -1;
}

int mixer_close(void)
{
	int rc = 0;

	if (handle) {
		rc = snd_mixer_close(handle);
		handle = NULL;
	}

#if 0 /* DEBUG */
	if (rc < 0) {
		fprintf(stderr, "%s: ERROR: %s\n", __func__, snd_strerror(rc));
	}
#endif

	return rc;
}

int mixer_get_volume(void)
{
	long volume;
	long min;
	long max;

	snd_mixer_selem_get_playback_volume_range(elem, &min, &max);

	if (snd_mixer_selem_get_playback_volume(elem, 0, &volume) < 0) {
		fprintf(stderr, "%s: Error get volume\n", __func__);
		return 0;
	}

	/* Make the value bound from 0 to 100 */
	volume -= min;
	max -= min;
	min = 0;

	/*	fprintf(stderr, "%s: volume %ld (%ld%%)\n", __func__, volume, (100 * volume) / max);*/
	return (int) (100 * volume) / max;
}

int mixer_set_volume(const int value)
{
	int rc;
	long min;
	long max;
	long volume = value;

	snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
	rc = snd_mixer_selem_set_playback_volume_all(elem, volume * max / 100);
	if (rc < 0) {
		fprintf(stderr, "Unable to set volume\n");
	}
#if 0
	 else {
		fprintf(stderr, "%s: Volume range <%ld,%ld>\n", __func__, min, max);
		fprintf(stderr, "%s: Volume set to: %d\n", __func__, mixer_get_volume());
	}
#endif

	return value;
}


