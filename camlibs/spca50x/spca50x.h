/****************************************************************/
/* spca50x.h - Gphoto2 library for cameras with sunplus spca50x */
/*             chips                                            */
/*                                                              */
/* Copyright � 2002 Till Adam                                   */
/*                                                              */
/* Author: Till Adam <till@adam-lilienthal.de>                  */
/*                                                              */
/* This library is free software; you can redistribute it       */
/* and/or modify it under the terms of the GNU Library General  */
/* Public License as published by the Free Software Foundation; */
/* either version 2 of the License, or (at your option) any     */
/* later version.                                               */
/*                                                              */
/* This library is distributed in the hope that it will be      */
/* useful, but WITHOUT ANY WARRANTY; without even the implied   */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR      */
/* PURPOSE.  See the GNU Library General Public License for     */
/* more details.                                                */
/*                                                              */
/* You should have received a copy of the GNU Library General   */
/* Public License along with this library; if not, write to the */
/* Free Software Foundation, Inc., 59 Temple Place - Suite 330, */
/* Boston, MA 02111-1307, USA.                                  */
/****************************************************************/

#ifndef __SPCA50X_H__
#define __SPCA50X_H__
#include <gphoto2-camera.h>

#define SPCA50X_FAT_PAGE_SIZE 0x100
#define SPCA50X_FILE_TYPE_IMAGE 0x00
#define SPCA50X_FILE_TYPE_AVI 0x01

#define CHECK(result) {int res; res = result; if (res < 0) return (res);}

struct SPCA50xFile
{
	char *name;
	int width;
	int height;
	int fat_start;
	int fat_end;
	u_int8_t *fat;
	int mime_type;
};

typedef enum {
           BRIDGE_SPCA500,
	   BRIDGE_SPCA504
} SPCA50xBridgeChip;

struct _CameraPrivateLibrary
{
	GPPort *gpdev;
	int dirty:1;
	int storage_media_mask;
	u_int8_t fw_rev;
	SPCA50xBridgeChip bridge;
	int num_files;
	int num_images;
	int num_movies;
	int num_fats;
	int size_used;
	int size_free;
	u_int8_t *flash_toc;
	u_int8_t *fats;
	struct SPCA50xFile *files;
};

#define SPCA50X_SDRAM 0x01
#define SPCA50X_FLASH 0x02
#define SPCA50X_CARD  0x04

int spca50x_get_firmware_revision (CameraPrivateLibrary *lib);
int spca50x_detect_storage_type (CameraPrivateLibrary *lib);
int spca50x_reset (CameraPrivateLibrary * lib);
int spca50x_capture (CameraPrivateLibrary * lib);
int yuv2rgb (int y, int u, int v, int *r, int *g, int *b);

#endif /* __SPCA50X_H__ */
