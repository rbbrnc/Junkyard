/*
 * mixer_snd.c
 *
 * Audio Volume Applet -- Soundcard mixer interface
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>

#include <sys/soundcard.h>

#define MIXER_DEVICE "/dev/mixer"

static int mixer_fd = -1;

int mixer_open(void)
{
	if (mixer_fd == -1) {
		mixer_fd = open(MIXER_DEVICE, O_RDWR);
	}

	return mixer_fd;
}

int mixer_close(void)
{
	int rc = 0;

	if (mixer_fd != -1) {
		rc = close(mixer_fd);
		mixer_fd = -1;
	}

	return rc;
}

int mixer_set_volume(const int value)
{
	int volume;

	if (value > 100) {
		volume = 100 | (100 << 8);
	} else if (value < 0) {
		volume = 0;
	} else {
		volume = value | (value << 8);
	}

	if (ioctl(mixer_fd, SOUND_MIXER_WRITE_VOLUME, &volume) == -1) {
		fprintf(stderr, "Unable to set volume\n");
	}

	return value;
}

int mixer_get_volume(void)
{
	int volume;

	if (ioctl(mixer_fd, SOUND_MIXER_READ_VOLUME, &volume) == -1) {
		fprintf(stderr, "Unable to get volume\n");
		return -1;
	}

	return ((volume & 0xff) + ((volume >> 8) & 0xff) ) / 2;
}
