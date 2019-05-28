/*
 *  Copyright 2017-2018 NXP
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or late.
 *
 */

/*
 * @file mx8_v4l2_cap_drm.c
 *
 * @brief MX8 Video For Linux 2 driver test application
 *
 */

/* Standard Include Files */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <poll.h>
#include <pthread.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <drm/drm.h>
#include <drm/drm_mode.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <linux/videodev2.h>

#include "soc_check.h"

/* Helper Macro */
#define NUM_PLANES				3
#define TEST_BUFFER_NUM			3
#define NUM_SENSORS				8
#define NUM_CARDS				8
#define DEFAULT					4
#define WIDTH					1280
#define HEIGHT					800

#define INFO_LEVEL				4
#define DBG_LEVEL				5
#define ERR_LEVEL				6

#define v4l2_printf(LEVEL, fmt, args...)  \
do {                                      \
	printf("(%s:%d): "fmt, __func__, __LINE__, ##args);   \
} while(0)

#define v4l2_info(fmt, args...)  \
		v4l2_printf(INFO_LEVEL,"\x1B[36m"fmt"\e[0m", ##args)
#define v4l2_dbg(fmt, args...)   \
		v4l2_printf(DBG_LEVEL, "\x1B[33m"fmt"\e[0m", ##args)
#define v4l2_err(fmt, args...)   \
	    v4l2_printf(ERR_LEVEL, "\x1B[31m"fmt"\e[0m", ##args)

/*
 * DRM modeset releated data structure definition
 */
struct drm_buffer {
	void *fb_base;

	__u32 width;
	__u32 height;
	__u32 stride;
	__u32 size;

	__u32 handle;
	__u32 buf_id;
};

struct drm_device {
	int drm_fd;

	__s32 crtc_id;
	__s32 card_id;
	uint32_t conn_id;

	__u32 bits_per_pixel;
	__u32 bytes_per_pixel;

	drmModeModeInfo mode;
	drmModeCrtc *saved_crtc;

	/* double buffering */
	struct drm_buffer buffers[2];
	__u32 nr_buffer;
	__u32 front_buf;
};

struct media_dev {
	struct drm_device *drm_dev;
	__u32 total_frames_cnt;
};

/*
 * Global variable definition
 */
static sigset_t sigset_v;
static uint32_t log_level;
static uint32_t g_cam;
static uint32_t g_cam_num;
static uint32_t g_num_buffers;
static uint32_t g_num_frames;
static uint32_t g_num_planes;
static uint32_t g_capture_mode;
static uint32_t g_camera_framerate;
static uint32_t g_cap_fmt;
static uint32_t g_mem_type;
static uint32_t g_out_width;
static uint32_t g_out_height;
static uint32_t g_cap_ow;
static uint32_t g_cap_oh;

static bool g_saved_to_file;
static bool g_performance_test;
static bool quitflag;

static char g_v4l_device[NUM_SENSORS][100];
static char g_saved_filename[NUM_SENSORS][100];
static char g_fmt_name[10];

static bool g_cap_hfilp;
static bool g_cap_vfilp;
static int32_t g_cap_alpha;

/*
 *
 */
static int signal_thread(void *arg)
{
	int sig;

	pthread_sigmask(SIG_BLOCK, &sigset_v, NULL);

	while (1) {
		sigwait(&sigset_v, &sig);
		if (sig == SIGINT) {
			v4l2_info("Ctrl-C received. Exiting.\n");
			exit(0);
		} else {
			v4l2_err("Unknown signal. Still exiting\n");
		}
		quitflag = 1;
		break;
	}
	return 0;
}

static void global_vars_init(void)
{
	int i;

	for (i = 0; i < NUM_SENSORS; i++) {
		sprintf(g_v4l_device[i], "/dev/video%d", i);
		sprintf(g_saved_filename[i], "%d.", i);
	}
	sprintf(g_fmt_name, "XR24");

	log_level = DEFAULT;
	g_cam_num = 0;
	g_num_planes = 1;
	g_num_buffers = TEST_BUFFER_NUM;
	g_num_frames = 300;
	g_out_width = WIDTH;
	g_out_height = HEIGHT;
	g_capture_mode = 0;
	g_camera_framerate = 30;
	g_cap_fmt = V4L2_PIX_FMT_XBGR32;
	g_mem_type = V4L2_MEMORY_MMAP;

	quitflag = false;
	g_saved_to_file = false;
	g_performance_test = false;

	g_cap_hfilp = false;
	g_cap_vfilp = false;
	g_cap_alpha = 0;
}

static void print_help(const char *name)
{
	v4l2_info("CSI Video4Linux capture Device Test\n"
	       "Syntax: %s\n"
	       " -num <numbers of frame need to be captured>\n"
	       " -of save to file \n"
	       " -l <device support list>\n"
	       " -log <log level, 6 will show all message>\n"
	       " -cam <device index> 0bxxxx,xxxx\n"
	       " -d \"/dev/videoX\" if user use this option, -cam should be 1\n"
		   " -p test performance, need to combine with \"-of\" option\n"
		   " -m <mode> specify camera sensor capture mode(mode:0, 1, 2, 3, 4)\n"
		   " -fr <frame_rate> support 15fps and 30fps\n"
		   " -fmt <format> support XR24, AR24, RGBP, RGB3, BGR3, YUV4, YM24, YUYV and NV12, only XR24, AR24 and RGBP support playback\n"
		   " -ow <width> specify output width\n"
		   " -oh <height> specify output height\n"
		   " -hflip <num> enable horizontal flip, num: 0->disable or 1->enable\n"
		   " -vflip <num> enable vertical flip, num: 0->disable or 1->enable\n"
		   " -alpha <num> enable and set global alpha for camera, num equal to 0~255\n"
	       "example:\n"
	       "./mx8_cap -cam 1        capture data from video0 and playback\n"
	       "./mx8_cap -cam 3        capture data from video0/1 and playback\n"
	       "./mx8_cap -cam 7 -of    capture data from video0~2 and save to 0~2.BX24\n"
	       "./mx8_cap -cam 255 -of  capture data from video0~7 and save to 0~7.BX24\n"
	       "./mx8_cap -cam 0xff -of capture data from video0~7 and save to 0~7.BX24\n"
	       "./mx8_cap -cam 1 -fmt NV12 -of capture data from video0 and save to 0.NV12\n"
	       "./mx8_cap -cam 1 -of -p test video0 performace\n", name);
}

static __u32 to_fourcc(char fmt[])
{
	__u32 fourcc;

	if (!strcmp(fmt, "BX24"))
		fourcc = V4L2_PIX_FMT_XRGB32;
	else if (!strcmp(fmt, "BA24"))
		fourcc = V4L2_PIX_FMT_ARGB32;
	else if (!strcmp(fmt, "BGR3"))
		fourcc = V4L2_PIX_FMT_BGR24;
	else if (!strcmp(fmt, "RGB3"))
		fourcc = V4L2_PIX_FMT_RGB24;
	else if (!strcmp(fmt, "RGBP"))
		fourcc = V4L2_PIX_FMT_RGB565;
	else if (!strcmp(fmt, "YUV4"))
		fourcc = V4L2_PIX_FMT_YUV32;
	else if (!strcmp(fmt, "YUYV"))
		fourcc = V4L2_PIX_FMT_YUYV;
	else if (!strcmp(fmt, "NV12"))
		fourcc = V4L2_PIX_FMT_NV12;
	else if (!strcmp(fmt, "YM24"))
		fourcc = V4L2_PIX_FMT_YUV444M;
	else if (!strcmp(fmt, "XR24"))
		fourcc = V4L2_PIX_FMT_XBGR32;
	else if (!strcmp(fmt, "AR24"))
		fourcc = V4L2_PIX_FMT_ABGR32;
	else {
		v4l2_err("Not support format, set default to XR24\n");
		fourcc = V4L2_PIX_FMT_XBGR32;
	}
	return fourcc;
}


static int parse_cmdline(int argc, const char *argv[])
{
	int i;

/*
	if (argc < 2) {
		print_help(argv[0]);
		return -1;
	}
*/
	print_help(argv[0]);

	/* Parse command line */
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-cam") == 0) {
			unsigned long mask;
			mask = strtoul(argv[++i], NULL, 0);
			g_cam = mask;
		} else if (strcmp(argv[i], "-num") == 0) {
			g_num_frames = atoi(argv[++i]);
		} else if (strcmp(argv[i], "-help") == 0) {
			print_help(argv[0]);
			return -1;
		} else if (strcmp(argv[i], "-of") == 0) {
			g_saved_to_file = true;
		} else if (strcmp(argv[i], "-p") == 0) {
			g_performance_test = true;
		} else if (strcmp(argv[i], "-log") == 0) {
			log_level = atoi(argv[++i]);
		} else if (strcmp(argv[i], "-m") == 0) {
			g_capture_mode = atoi(argv[++i]);
		} else if (strcmp(argv[i], "-fr") == 0) {
			g_camera_framerate = atoi(argv[++i]);
		} else if (strcmp(argv[i], "-fmt") == 0) {
			char temp_fmt[10];
			strcpy(temp_fmt, argv[++i]);
			g_cap_fmt = to_fourcc(temp_fmt);
		} else if (strcmp(argv[i], "-d") == 0) {
			if (g_cam != 1) {
				print_help(argv[0]);
				return -1;
			}
			strcpy(g_v4l_device[0], argv[++i]);
		} else if (strcmp(argv[i], "-ow") == 0) {
			g_cap_ow = atoi(argv[++i]);
		} else if (strcmp(argv[i], "-oh") == 0) {
			g_cap_oh = atoi(argv[++i]);
		} else if (strcmp(argv[i], "-hflip") == 0) {
			g_cap_hfilp = atoi(argv[++i]);
		} else if (strcmp(argv[i], "-vflip") == 0) {
			g_cap_vfilp = atoi(argv[++i]);
		} else if (strcmp(argv[i], "-alpha") == 0) {
			g_cap_alpha = atoi(argv[++i]);
		} else {
			print_help(argv[0]);
			return -1;
		}
	}
	return 0;
}

static void get_fmt_name(uint32_t fourcc)
{
	switch (fourcc) {
	case V4L2_PIX_FMT_RGB565:
		strcpy(g_fmt_name, "RGBP");
		break;
	case V4L2_PIX_FMT_RGB24:
		strcpy(g_fmt_name, "RGB3");
		break;
	case V4L2_PIX_FMT_XRGB32:
		strcpy(g_fmt_name, "BX24");
		break;
	case V4L2_PIX_FMT_BGR24:
		strcpy(g_fmt_name, "BGR3");
		break;
	case V4L2_PIX_FMT_ARGB32:
		strcpy(g_fmt_name, "BA24");
		break;
	case V4L2_PIX_FMT_YUYV:
		strcpy(g_fmt_name, "YUYV");
		break;
	case V4L2_PIX_FMT_YUV32:
		strcpy(g_fmt_name, "YUV4");
		break;
	case V4L2_PIX_FMT_NV12:
		strcpy(g_fmt_name, "NV12");
		break;
	case V4L2_PIX_FMT_YUV444M:
		strcpy(g_fmt_name, "YM24");
		break;
	case V4L2_PIX_FMT_XBGR32:
		strcpy(g_fmt_name, "XR24");
		break;
	case V4L2_PIX_FMT_ABGR32:
		strcpy(g_fmt_name, "AR24");
		break;
	default:
		strcpy(g_fmt_name, "null");
	}
}

static int media_device_alloc(struct media_dev *media)
{
	struct drm_device *drm;

	if (!g_saved_to_file) {
		drm = malloc(sizeof(*drm));
		if (drm == NULL) {
			v4l2_err("alloc DRM device fail\n");
			return -ENOMEM;
		}
		memset(drm, 0, sizeof(*drm));
		media->drm_dev = drm;
	}
	return 0;
}

static void media_device_free(struct media_dev *media)
{
	if (media->drm_dev && !g_saved_to_file)
		free(media->drm_dev);
}

static int open_drm_device(struct drm_device *drm)
{
	char dev_name[15];
	uint64_t has_dumb;
	int fd, i;

	i = 0;
loop:
	sprintf(dev_name, "/dev/dri/card%d", i++);

	fd = open(dev_name, O_RDWR | O_CLOEXEC | O_NONBLOCK);
	if (fd < 0) {
		v4l2_err("Open %s fail\n", dev_name);
		return -1;
	}

	if (drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 ||
	    !has_dumb) {
		v4l2_err("drm device '%s' does not support dumb buffers\n", dev_name);
		close(fd);
		goto loop;
	}
	drm->drm_fd = fd;
	drm->card_id = --i;

	v4l2_info("Open %s success\n", dev_name);

	return 0;
}

static void close_drm_device(int drm_fd)
{
	if (drm_fd > 0)
		close(drm_fd);
}

static int media_device_open(struct media_dev *media)
{
	struct drm_device *drm = media->drm_dev;
	int ret = 0;

	if (!g_saved_to_file)
		ret = open_drm_device(drm);

	if (ret < 0)
		return ret;

	return ret;
}

static void dump_drm_clients(const int dev_num)
{
	char cmd[50];

	sprintf(cmd, "cat /sys/kernel/debug/dri/%d/clients", dev_num);

	printf("========================================================\n");
	system(cmd);
	printf("========================================================\n");
	printf("Please ensure there is no other master client\n");
	printf("========================================================\n");
}

static int modeset_find_crtc(struct drm_device *drm,
				drmModeRes *res, drmModeConnector *conn)
{
	drmModeEncoder *encoder;
	int drm_fd = drm->drm_fd;
	int crtc_id, j, i;

	for (i = 0; i < conn->count_encoders; i++) {
		encoder = drmModeGetEncoder(drm_fd, conn->encoders[i]);
		if (!encoder) {
			v4l2_err("can't retrieve encoders[%d]\n", i);
			continue;
		}

		for (j = 0; j < res->count_crtcs; j++) {
			if (encoder->possible_crtcs & (1 << j)) {
				crtc_id = res->crtcs[j];
				if (crtc_id > 0) {
					drm->crtc_id = crtc_id;
					drmModeFreeEncoder(encoder);
					return 0;
				}
			}
			crtc_id = -1;
		}

		if (j == res->count_crtcs && crtc_id == -1) {
			v4l2_err("cannot find crtc\n");
			drmModeFreeEncoder(encoder);
			continue;
		}
		drmModeFreeEncoder(encoder);
	}
	v4l2_err("cannot find suitable CRTC for connector[%d]\n", conn->connector_id);
	return -ENOENT;
}

static int adjust(__u32 fourcc)
{
	int bpp;

	switch(fourcc) {
		case V4L2_PIX_FMT_XRGB32:
		case V4L2_PIX_FMT_XBGR32:
		case V4L2_PIX_FMT_ARGB32:
		case V4L2_PIX_FMT_ABGR32:
			bpp = 32;
			break;
		case V4L2_PIX_FMT_RGB565:
			bpp = 16;
			break;
		default:
			bpp = 32;
	}
	return bpp;
}

static int drm_create_fb(int fd, int index, struct drm_buffer *buf)
{
	struct drm_mode_create_dumb creq;
	struct drm_mode_destroy_dumb dreq;
	struct drm_mode_map_dumb mreq;
	int ret;

	memset(&creq, 0, sizeof(creq));
	creq.width = buf->width;
	creq.height = buf->height;
	creq.bpp = adjust(g_cap_fmt);

	ret = drmIoctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq);
	if (ret < 0) {
		v4l2_err("cannot create dumb buffer[%d]\n", index);
		return ret;
	}

	buf->stride = creq.pitch;
	buf->size = creq.size;
	buf->handle = creq.handle;

	ret = drmModeAddFB(fd, buf->width, buf->height, creq.bpp, creq.bpp,
				buf->stride, buf->handle, &buf->buf_id);

	if (ret < 0) {
		v4l2_err("Add framebuffer (%d) fail\n", index);
		goto destroy_fb;
	}

	memset(&mreq, 0, sizeof(mreq));
	mreq.handle = buf->handle;
	ret = drmIoctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
	if (ret) {
		v4l2_err("Map buffer[%d] dump ioctl fail\n", index);
		goto remove_fb;
	}

	buf->fb_base = mmap(0, buf->size, PROT_READ | PROT_WRITE, MAP_SHARED,
							fd, mreq.offset);
	if (buf->fb_base == MAP_FAILED) {
		v4l2_err("Cannot mmap dumb buffer[%d]\n", index);
		goto remove_fb;
	}
	memset(buf->fb_base, 0xFF, buf->size);

	return 0;

remove_fb:
	drmModeRmFB(fd, buf->buf_id);
destroy_fb:
	memset(&dreq, 0, sizeof(dreq));
	dreq.handle = buf->handle;
	drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
	v4l2_err("Create DRM buffer[%d] fail\n", index);
	return ret;
}

static void drm_destroy_fb(int fd, int index, struct drm_buffer *buf)
{
	struct drm_mode_destroy_dumb dreq;

	munmap(buf->fb_base, buf->size);
	drmModeRmFB(fd, buf->buf_id);

	memset(&dreq, 0, sizeof(dreq));
	dreq.handle = buf->handle;
	drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
}

static int modeset_setup_dev(struct drm_device *drm,
				drmModeRes *res, drmModeConnector *conn)
{
	printf("%s\n", __func__);
	struct drm_buffer *buf = drm->buffers;
	int i, ret;

	ret = modeset_find_crtc(drm, res, conn);
	if (ret < 0)
		return ret;

	memcpy(&drm->mode, &conn->modes[0], sizeof(drm->mode));
	/* Double buffering */
	for (i = 0; i < 2; i++) {
		buf[i].width  = conn->modes[0].hdisplay;
		buf[i].height = conn->modes[0].vdisplay;
		ret = drm_create_fb(drm->drm_fd, i, &buf[i]);
		if (ret < 0) {
			while(i)
				drm_destroy_fb(drm->drm_fd, i - 1, &buf[i-1]);
			return ret;
		}
		v4l2_dbg("DRM bufffer[%d] addr=0x%p size=%d w/h=(%d,%d) buf_id=%d\n",
				 i, buf[i].fb_base, buf[i].size,
				 buf[i].width, buf[i].height, buf[i].buf_id);
	}
	drm->bits_per_pixel = adjust(g_cap_fmt);
	drm->bytes_per_pixel = drm->bits_per_pixel >> 3;
	return 0;
}

static int drm_device_prepare(struct drm_device *drm)
{
	drmModeRes *res;
	drmModeConnector *conn;
	int drm_fd = drm->drm_fd;
	int ret, i;

	ret = drmSetMaster(drm_fd);
	if (ret < 0) {
		dump_drm_clients(drm->card_id);
		return ret;
	}

	res = drmModeGetResources(drm_fd);
	if (res == NULL) {
		v4l2_err("Cannot retrieve DRM resources\n");
		drmDropMaster(drm_fd);
		return -errno;
	}

	/* iterate all connectors */
	for (i = 0; i < res->count_connectors; i++) {
		/* get information for each connector */
		conn = drmModeGetConnector(drm_fd, res->connectors[i]);
		if (!conn) {
			v4l2_err("Cannot retrieve DRM connector %u:%u (%d)\n",
				i, res->connectors[i], errno);
			continue;
		}

		/* valid connector? */
		if (conn->connection != DRM_MODE_CONNECTED ||
					conn->count_modes == 0) {
			drmModeFreeConnector(conn);
			continue;
		}

		/* find a valid connector */
		drm->conn_id = conn->connector_id;
		ret = modeset_setup_dev(drm, res, conn);
		if (ret < 0) {
			v4l2_err("mode setup device environment fail\n");
			drmDropMaster(drm_fd);
			drmModeFreeConnector(conn);
			drmModeFreeResources(res);
			return ret;
		}
		drmModeFreeConnector(conn);
	}
	drmModeFreeResources(res);
	return 0;
}

static void media_device_cleanup(struct media_dev *media)
{
	int i, fd;

	if (!g_saved_to_file) {
		fd = media->drm_dev->drm_fd;
		drmDropMaster(fd);
		drm_destroy_fb(fd, 0, &media->drm_dev->buffers[0]);
		drm_destroy_fb(fd, 1, &media->drm_dev->buffers[1]);
	}
}

static int media_device_prepare(struct media_dev *media)
{
	struct drm_device *drm = media->drm_dev;
	int ret;

	if (!g_saved_to_file) {
		ret = drm_device_prepare(drm);
		if (ret < 0) {
			drmDropMaster(drm->drm_fd);
			return ret;
		}
	}

	return 0;
}

static int media_device_start(struct media_dev *media)
{
	struct drm_device *drm = media->drm_dev;
	struct drm_buffer *buf;
	int i, ret;

	if (!g_saved_to_file) {
		buf = &drm->buffers[drm->front_buf];
		ret = drmModeSetCrtc(drm->drm_fd, drm->crtc_id, buf->buf_id,
							 0, 0, &drm->conn_id, 1, &drm->mode);
		if (ret < 0) {
			v4l2_err("buffer[%d] set CRTC fail\n", buf->buf_id);
			return ret;
		}
		v4l2_dbg("crtc_id=%d conn_id=%d buf_id=%d\n",
				 drm->crtc_id, drm->conn_id,
				 drm->buffers[drm->front_buf].buf_id);
	}

	return 0;
}

static int display_on_screen(int ch, struct media_dev *media, unsigned int color)
{
	struct drm_device *drm = media->drm_dev;
	struct drm_buffer *buf = &drm->buffers[drm->front_buf^1];
	int buf_id = ch;
	static int enter_count = 0;
	int i, j, ret;

	for (j = 0; j < buf->height; j++) {
		for (i = 0; i < buf->width; i++) {
			*((unsigned char *)(buf->fb_base + (j * buf->width * 4) + i * 4 + 0)) = color & 0xff;
			*((unsigned char *)(buf->fb_base + (j * buf->width * 4) + i * 4 + 1)) = (color >> 8) & 0xff;
			*((unsigned char *)(buf->fb_base + (j * buf->width * 4) + i * 4 + 2)) = (color >> 16) & 0xff;
			*((unsigned char *)(buf->fb_base + (j * buf->width * 4) + i * 4 + 3)) = (color >> 24) & 0xff;
		}
	}

	if (!(++enter_count % g_cam_num)) {
		ret = drmModeSetCrtc(drm->drm_fd, drm->crtc_id, buf->buf_id, 0, 0,
					&drm->conn_id, 1, &drm->mode);
		if (ret < 0) {
			v4l2_err("Set Crtc fail\n");
			return ret;
		}
		enter_count = 0;
		drm->front_buf ^= 1;

		// v4l2_dbg("crtc_id=%d conn_id=%d buf_id=%d\n", drm->crtc_id, drm->conn_id, buf->buf_id);
	}
	return 0;
}


static int redraw(struct media_dev *media, unsigned int color)
{

	display_on_screen(0, media, color);

	return 0;
}

static void media_device_close(struct media_dev *media)
{
	if (!g_saved_to_file)
		close_drm_device(media->drm_dev->drm_fd);

}

/*
 * Main function
 */
int main(int argc, const char *argv[])
{
	struct media_dev media;
	char *soc_list[] = { "i.MX8QM", "i.MX8QXP", " " };
	int ret;

	ret = soc_version_check(soc_list);
	if (ret == 0) {
		v4l2_err("not supported on current soc\n");
		return 0;
	}

	global_vars_init();

	pthread_t sigtid;
	sigemptyset(&sigset_v);
	sigaddset(&sigset_v, SIGINT);
	pthread_sigmask(SIG_BLOCK, &sigset_v, NULL);
	pthread_create(&sigtid, NULL, (void *)&signal_thread, NULL);

	ret = parse_cmdline(argc, argv);
	if (ret < 0) {
		v4l2_err("%s failed to parse arguments\n", argv[0]);
		return ret;
	}

	ret = media_device_alloc(&media);
	if (ret < 0) {
		v4l2_err("No enough memory\n");
		return -ENOMEM;
	}

	ret = media_device_open(&media);
	if (ret < 0)
		goto free;
	printf("after media_device_open()\n");

	ret = media_device_prepare(&media);
	if (ret < 0)
		goto close;
	printf("after media_device_prepare()\n");

	ret = media_device_start(&media);
	if (ret < 0)
		goto cleanup;
	printf("after media_device_start()\n");


	// getchar();
	unsigned int color[] = {0xFF, 0xFF00, 0xFF0000, 0x00FFFF, 0xFF00FF, 0xFFFF00, 0xFFFFFF};
	int colorIndex = 0;

	while(!quitflag) {
		ret = redraw(&media, color[colorIndex]);
		if (ret < 0)
			printf("redraw error\n");
		sleep(1);

		if (++colorIndex == sizeof(color)/sizeof(unsigned int))
			colorIndex = 0;

	}


cleanup:
	media_device_cleanup(&media);
close:
	media_device_close(&media);
free:
	media_device_free(&media);
	if (ret == 0)
		v4l2_info("=*= success =*=\n");
	return ret;
}
