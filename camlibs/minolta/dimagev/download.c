/**********************************************************************
*       Minolta Dimage V digital camera communication library         *
*               Copyright (C) 2000 Gus Hartmann                       *
*                                                                     *
*    This program is free software; you can redistribute it and/or    *
*    modify it under the terms of the GNU General Public License as   *
*    published by the Free Software Foundation; either version 2 of   *
*    the License, or (at your option) any later version.              *
*                                                                     *
*    This program is distributed in the hope that it will be useful,  *
*    but WITHOUT ANY WARRANTY; without even the implied warranty of   *
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    *
*    GNU General Public License for more details.                     *
*                                                                     *
*    You should have received a copy of the GNU General Public        *
*    License along with this program; if not, write to the Free       *
*    Software Foundation, Inc., 59 Temple Place, Suite 330,           *
*    Boston, MA 02111-1307 USA                                        *
*                                                                     *
**********************************************************************/

/* $Id$ */

#include "dimagev.h"

int dimagev_get_picture(dimagev_t *dimagev, int file_number, CameraFile *file) {
	int length, total_packets, i;
	dimagev_packet *p, *r;
	unsigned char char_buffer, command_buffer[3];
#ifdef _gphoto_exif_
	exifparser exifdat;
#endif

	if ( dimagev->data->host_mode != 1 ) {

		dimagev->data->host_mode = 1;

		if ( dimagev_send_data(dimagev) == GP_ERROR ) {
			if ( dimagev->debug != 0 ) {
				gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_picture::unable to set host mode");
			}
			return GP_ERROR;
		}
	}

	gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_picture::file_number is %d", file_number);

	/* Maybe check if it exists? Check the file type? */
	
	/* First make the command packet. */
	command_buffer[0] = 0x04;
	command_buffer[1] = (unsigned char)( file_number / 256 );
	command_buffer[2] = (unsigned char)( file_number % 256 );
	if ( ( p = dimagev_make_packet(command_buffer, 3, 0) ) == NULL ) {
		if ( dimagev->debug != 0 ) {
			gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_picture::unable to allocate command packet");
		}
		return GP_ERROR;
	}

	if ( gpio_write(dimagev->dev, p->buffer, p->length) == GPIO_ERROR ) {
		if ( dimagev->debug != 0 ) {
			gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_picture::unable to send set_data packet");
		}
		return GP_ERROR;
	} else if ( gpio_read(dimagev->dev, &char_buffer, 1) == GPIO_ERROR ) {
		if ( dimagev->debug != 0 ) {
			perror("dimagev_get_picture::no response from camera");
		}
		return GP_ERROR;
	}
		
	free(p);

	switch ( char_buffer ) {
		case DIMAGEV_ACK:
			break;
		case DIMAGEV_NAK:
			if ( dimagev->debug != 0 ) {
				gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_picture::camera did not acknowledge transmission\n");
			}
			return GP_ERROR;
			break;
		case DIMAGEV_CAN:
			if ( dimagev->debug != 0 ) {
				gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_picture::camera cancels transmission\n");
			}
			return GP_ERROR;
			break;
		default:
			if ( dimagev->debug != 0 ) {
				gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_picture::camera responded with unknown value %x\n", char_buffer);
			}
			return GP_ERROR;
			break;
	}

	if ( ( p = dimagev_read_packet(dimagev) ) == NULL ) {
		if ( dimagev->debug != 0 ) {
			gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_picture::unable to read packet");
		}
		return GP_ERROR;
	}

	if ( ( r = dimagev_strip_packet(p) ) == NULL ) {
		if ( dimagev->debug != 0 ) {
			gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_picture::unable to strip packet");
		}
		return GP_ERROR;
	}
		
	free(p);

	total_packets = r->buffer[0];
	length = ( r->length - 1 );

	/* Allocate an extra byte just in case. */
	if ( ( file->data = malloc((993 * total_packets) + 1) ) == NULL ) {
		if ( dimagev->debug != 0 ) {
			gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_picture::unable to allocate buffer for file");
		}
		return GP_ERROR;
	}

	memcpy(file->data, &(r->buffer[1]), r->length );
	file->size += ( r->length - 2 );

	free(r);

	for ( i = 0 ; i < ( total_packets -1 ) ; i++ ) {

		char_buffer=DIMAGEV_ACK;
		if ( gpio_write(dimagev->dev, &char_buffer, 1) == GPIO_ERROR ) {
			if ( dimagev->debug != 0 ) {
				gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_picture::unable to send ACK");
			}
			return GP_ERROR;
		}
	
		if ( ( p = dimagev_read_packet(dimagev) ) == NULL ) {
			if ( dimagev->debug != 0 ) {
				gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_picture::unable to read packet");
			}
			return GP_ERROR;
		}

		if ( ( r = dimagev_strip_packet(p) ) == NULL ) {
			if ( dimagev->debug != 0 ) {
				gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_picture::unable to strip packet");
			}
			return GP_ERROR;
		}
		
		free(p);

		memcpy(&( file->data[ ( file->size + 1) ] ), r->buffer, r->length );
		file->size += r->length;

		free(r);
	}

	file->size++;

	char_buffer=DIMAGEV_EOT;
	if ( gpio_write(dimagev->dev, &char_buffer, 1) == GPIO_ERROR ) {
		if ( dimagev->debug != 0 ) {
			gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_picture::unable to send ACK");
		}
		return GP_ERROR;
	}

	if ( gpio_read(dimagev->dev, &char_buffer, 1) == GPIO_ERROR ) {
		if ( dimagev->debug != 0 ) {
			perror("dimagev_get_picture::no response from camera");
		}
		return GP_ERROR;
	}
		
	switch ( char_buffer ) {
		case DIMAGEV_ACK:
			break;
		case DIMAGEV_NAK:
			if ( dimagev->debug != 0 ) {
				gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_picture::camera did not acknowledge transmission\n");
			}
			return GP_ERROR;
			break;
		case DIMAGEV_CAN:
			if ( dimagev->debug != 0 ) {
				gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_picture::camera cancels transmission\n");
			}
			return GP_ERROR;
			break;
		default:
			if ( dimagev->debug != 0 ) {
				gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_picture::camera responded with unknown value %x\n", char_buffer);
			}
			return GP_ERROR;
			break;
	}

	/* Now set the camera back into local mode. */
	dimagev->data->host_mode = 0;

	if ( dimagev_send_data(dimagev) == GP_ERROR ) {
		if ( dimagev->debug != 0 ) {
			gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_picture::unable to set host mode");
		}
		return GP_ERROR;
	}

	sleep(3);

#ifdef _gphoto_exif_
	exifdat.header = file->data;
	exifdat.data = file->data + 12 ;

	if ( stat_exif(&exifdat) != 0 ) {
		if ( dimagev->debug != 0 ) {
			gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_picture::unable to stat EXIF tags");
		}
		return GP_OK;
	}

	gpe_dump_exif(&exifdat);

#endif

	return GP_OK;
}

int dimagev_get_thumbnail(dimagev_t *dimagev, int file_number, CameraFile *file) {
	dimagev_packet *p, *r;
	unsigned char char_buffer, command_buffer[3], *ycrcb_data;

	if ( dimagev->data->host_mode != 1 ) {

		dimagev->data->host_mode = 1;

		if ( dimagev_send_data(dimagev) == GP_ERROR ) {
			if ( dimagev->debug != 0 ) {
				gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_thumbnail::unable to set host mode");
			}
			return GP_ERROR;
		}
	}

	/* First make the command packet. */
	command_buffer[0] = 0x0d;
	command_buffer[1] = (unsigned char)( file_number / 256 );
	command_buffer[2] = (unsigned char)( file_number % 256 );
	if ( ( p = dimagev_make_packet(command_buffer, 3, 0) ) == NULL ) {
		if ( dimagev->debug != 0 ) {
			gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_thumbnail::unable to allocate command packet");
		}
		return GP_ERROR;
	}

	if ( gpio_write(dimagev->dev, p->buffer, p->length) == GPIO_ERROR ) {
		if ( dimagev->debug != 0 ) {
			gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_thumbnail::unable to send set_data packet");
		}
		return GP_ERROR;
	} else if ( gpio_read(dimagev->dev, &char_buffer, 1) == GPIO_ERROR ) {
		if ( dimagev->debug != 0 ) {
			perror("dimagev_get_thumbnail::no response from camera");
		}
		return GP_ERROR;
	}
		
	free(p);

	switch ( char_buffer ) {
		case DIMAGEV_ACK:
			break;
		case DIMAGEV_NAK:
			if ( dimagev->debug != 0 ) {
				gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_thumbnail::camera did not acknowledge transmission\n");
			}
			return GP_ERROR;
			break;
		case DIMAGEV_CAN:
			if ( dimagev->debug != 0 ) {
				gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_thumbnail::camera cancels transmission\n");
			}
			return GP_ERROR;
			break;
		default:
			if ( dimagev->debug != 0 ) {
				gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_thumbnail::camera responded with unknown value %x\n", char_buffer);
			}
			return GP_ERROR;
			break;
	}

	if ( ( p = dimagev_read_packet(dimagev) ) == NULL ) {
		if ( dimagev->debug != 0 ) {
			gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_thumbnail::unable to read packet");
		}
		return GP_ERROR;
	}

	if ( ( r = dimagev_strip_packet(p) ) == NULL ) {
		if ( dimagev->debug != 0 ) {
			gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_thumbnail::unable to strip packet");
		}
		return GP_ERROR;
	}
		
	free(p);

	/* Unlike normal images, we are guaranteed 9600 bytes *exactly*. */

	/* Allocate an extra byte just in case. */
	if ( ( ycrcb_data = malloc(9600) ) == NULL ) {
		if ( dimagev->debug != 0 ) {
			gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_thumbnail::unable to allocate buffer for file");
		}
		return GP_ERROR;
	}

	memcpy(ycrcb_data, r->buffer, r->length );
	file->size +=  r->length - 1 ;

	free(r);

	while ( file->size < 9599 ) {

		char_buffer=DIMAGEV_ACK;
		if ( gpio_write(dimagev->dev, &char_buffer, 1) == GPIO_ERROR ) {
			if ( dimagev->debug != 0 ) {
				gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_thumbnail::unable to send ACK");
			}
			return GP_ERROR;
		}
	
		if ( ( p = dimagev_read_packet(dimagev) ) == NULL ) {
			if ( dimagev->debug != 0 ) {
				gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_thumbnail::unable to read packet");
			}
			return GP_ERROR;
		}

		if ( ( r = dimagev_strip_packet(p) ) == NULL ) {
			if ( dimagev->debug != 0 ) {
				gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_thumbnail::unable to strip packet");
			}
			return GP_ERROR;
		}
		
		free(p);

		memcpy(&( ycrcb_data[ ( file->size + 1) ] ), r->buffer, r->length );
		file->size += r->length;

		free(r);

		if ( dimagev->debug != 0 ) {
			gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_thumbnail::current file size is %d\n", file->size);
		}
	}

	file->size++;

	char_buffer=DIMAGEV_EOT;
	if ( gpio_write(dimagev->dev, &char_buffer, 1) == GPIO_ERROR ) {
		if ( dimagev->debug != 0 ) {
			gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_thumbnail::unable to send ACK");
		}
		return GP_ERROR;
	}

	if ( gpio_read(dimagev->dev, &char_buffer, 1) == GPIO_ERROR ) {
		if ( dimagev->debug != 0 ) {
			perror("dimagev_get_thumbnail::no response from camera");
		}
		return GP_ERROR;
	}
		
	switch ( char_buffer ) {
		case DIMAGEV_ACK:
			break;
		case DIMAGEV_NAK:
			if ( dimagev->debug != 0 ) {
				gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_thumbnail::camera did not acknowledge transmission\n");
			}
			return GP_ERROR;
			break;
		case DIMAGEV_CAN:
			if ( dimagev->debug != 0 ) {
				gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_thumbnail::camera cancels transmission\n");
			}
			return GP_ERROR;
			break;
		default:
			if ( dimagev->debug != 0 ) {
				gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_thumbnail::camera responded with unknown value %x\n", char_buffer);
			}
			return GP_ERROR;
			break;
	}

	/* Now set the camera back into local mode. */
	dimagev->data->host_mode = 0;

	if ( dimagev_send_data(dimagev) == GP_ERROR ) {
		if ( dimagev->debug != 0 ) {
			gp_debug_printf(GP_DEBUG_HIGH, "dimagev", "dimagev_get_thumbnail::unable to set host mode");
		}
		return GP_ERROR;
	}

	sleep(3);

	file->data = dimagev_ycbcr_to_ppm(ycrcb_data);
	file->size = 14413;

	return GP_OK;
}
