/* gphoto2-context.h
 *
 * Copyright (C) 2001 Lutz M�ller <lutz@users.sourceforge.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details. 
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __GPHOTO2_CONTEXT_H__
#define __GPHOTO2_CONTEXT_H__

#include <stdarg.h>

typedef struct _GPContext GPContext;

GPContext *gp_context_new (void);

void gp_context_ref   (GPContext *context);
void gp_context_unref (GPContext *context);

enum _GPContextFeedback {
	GP_CONTEXT_FEEDBACK_OK,
	GP_CONTEXT_FEEDBACK_CANCEL
};
typedef enum _GPContextFeedback GPContextFeedback;

/* Functions */
typedef void (* GPContextIdleFunc)     (GPContext *context, void *data);
typedef void (* GPContextErrorFunc)    (GPContext *context, const char *format,
					va_list args, void *data);
typedef void (* GPContextStatusFunc)   (GPContext *context, const char *format,
					va_list args, void *data);
typedef void (* GPContextMessageFunc)  (GPContext *context, const char *format,
					va_list args, void *data);
typedef GPContextFeedback (* GPContextQuestionFunc) (GPContext *context,
						     const char *format,
						     va_list args, void *data);
typedef GPContextFeedback (* GPContextCancelFunc)   (GPContext *context,
						     void *data);
typedef unsigned int (* GPContextProgressStartFunc)  (GPContext *context,
						      float target,
						      const char *format,
						      va_list args, void *data);
typedef void         (* GPContextProgressUpdateFunc) (GPContext *context,
						      unsigned int id,
						      float current,
						      void *data);
typedef void         (* GPContextProgressStopFunc)   (GPContext *context,
						      unsigned int id,
						      void *data);

/* Setting those functions (frontends) */
void gp_context_set_idle_func      (GPContext *context,
			            GPContextIdleFunc func,     void *data);
void gp_context_set_progress_funcs (GPContext *context,
				    GPContextProgressStartFunc  start_func,
				    GPContextProgressUpdateFunc update_func,
				    GPContextProgressStopFunc   stop_func,
				    void *data);
void gp_context_set_error_func     (GPContext *context,
				    GPContextErrorFunc func,    void *data);
void gp_context_set_status_func    (GPContext *context,
				    GPContextStatusFunc func,   void *data);
void gp_context_set_question_func  (GPContext *context,
				    GPContextQuestionFunc func, void *data);
void gp_context_set_cancel_func    (GPContext *context,
				    GPContextCancelFunc func,   void *data);
void gp_context_set_message_func   (GPContext *context,
				    GPContextMessageFunc func,  void *data);

/* Calling those functions (backends) */
void gp_context_idle     (GPContext *context);
void gp_context_error    (GPContext *context, const char *format, ...);
void gp_context_status   (GPContext *context, const char *format, ...);
void gp_context_message  (GPContext *context, const char *format, ...);
GPContextFeedback gp_context_question (GPContext *context, const char *format,
				       ...);
GPContextFeedback gp_context_cancel   (GPContext *context);
unsigned int gp_context_progress_start  (GPContext *context, float target,
					 const char *format, ...);
void         gp_context_progress_update (GPContext *context, unsigned int id,
					 float current);
void         gp_context_progress_stop   (GPContext *context, unsigned int id);

#endif /* __GPHOTO2_CONTEXT_H__ */
