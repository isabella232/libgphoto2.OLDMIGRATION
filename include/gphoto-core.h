/* 	Header file for gPhoto 0.5-Dev

	Author: Scott Fritzinger <scottf@unr.edu>

	This library is covered by the LGPL.
*/


/* Save/retrieve library or front-end configuration data. */
	int gp_get_setting (char *key, char *value);
	int gp_set_setting (char *key, char *value);


/* Functions for the front-ends
   ---------------------------------------------------------------- */

/* Initialize/Quit the gPhoto library */
	int gp_init();
	int gp_exit();

/* Camera port operations */
	int gp_port_set (CameraPortSettings settings);

/* Camera operations */
	/* Retrieve number of cameras */
	int gp_camera_count ();

	/* Retrieve camera name */
	int gp_camera_name (int camera_number, char *camera_name);

	/* Set the current camera library */
	int gp_camera_set (int camera_number);

	/* Retrieve the camera abilities */
	int gp_camera_abilities (CameraAbilities *abilities);

	/* Init/Exit, Open or close a camera */
	int gp_camera_init (CameraInit *init);
	int gp_camera_exit ();

	int gp_camera_open ();
	int gp_camera_close ();

/* Folder operations */
	/* Retrieve the number of folders on the camera */
	int gp_folder_count ();

	/* Retrieve the name of a particular folder */
	int gp_folder_name (int folder_number, char *folder_name);

	/* Set the current folder */
	int gp_folder_set (int folder_number);

/* File operations */
	/* Retrieve the number of files in the current folder */
	int gp_file_count ();

	/* Download a file or a preview from the camera from the current folder*/
	int gp_file_get (int file_number, CameraFile *file);
	int gp_file_get_preview (int file_number, CameraFile *preview);

	/* Delete a file from the current folder */
	int gp_file_delete (int file_number);

	/* Saves image to "filename" (full path and filename) */
	int gp_file_save (int file_number, char *filename);

	/* ---- These next 2 are just utilities ---- */

	/* Allocates a new CameraFile */
	CameraFile* gp_file_new ();

	/* Frees a CameraFile's components */
	int gp_file_clean (CameraFile *file);

	/* Frees a CameraFile from memory */
	int gp_file_free (CameraFile *file);

/* Camera configuration operations */
	/* Retrieve and set configuration options for the camera */
	int gp_config_get (char *config_dialog_filename);

	/* The config dialog will be stored in a plainttext file. The filename is
	   returned to the gphoto-core, which will pass it to the front-end. 
	   The format is still under discussion. XML leads the way though intpotential
	   formats. we need a draft though.

	   Something like (note: my XML is awful, so no standards here) :
		<page name="Port/Speed">
			<menu name="Port" value="/dev/ttyS0">
			<menu name="Port" value="/dev/ttyS1">
			<menu name="Port" value="/dev/ttyS2">
			<menu name="Port" value="/dev/ttyS3">
			- or -
			<text name="Other Port">
		</page>
		<page name="General">
			<text name="Camera Name">
			<label value="Quality">
			<radio name="Quality" value="High">
			<radio name="Quality" value="Normal">
			<radio name="Quality" value="Low">
		</page>
		<page name="Flash/Lens">
			<label value="Lens Mode">
			<radio name="Mode" value="Macro">
			<radio name="Mode" value="Normal">
			<group name="Flash">
				<radio name="Flash" value="Flash">
				<radio name="Flash" value="No Flash">
				<checkbox name="Red-eye Reduction">
			</group>
		</page>
	*/

	/* Send "key=value" pairs to the library, delimited by '\n' */
	int gp_config_set (char *config_settings);

/* Miscellaneous functions */
	/* Captures the current view. Basically, it takes a picture */
	int gp_capture (int type);

	/* Retrieves camera status (number of pictures, charge, etc...) */
	int gp_summary (char *summary);

	/* Retrieves usage of the camera library (config settings, problems, etc) */
	int gp_manual (char *manual);

	/* Retrieves information about the camera library (author, version, etc) */
	int gp_about (char *about);

/* Image Manipulation Functions */
	int gp_image_preview_rotate (int file_number, int degrees, CameraFile *new_file);
	int gp_image_preview_flip_h (int file_number, CameraFile *new_file);
	int gp_image_preview_flip_v (int file_number, CameraFile *new_file);

	int gp_image_rotate (int file_number, int degrees, CameraFile *new_file);
	int gp_image_scale (int file_number, int width, int height, CameraFile *new_file);
	int gp_image_flip_h (int file_number, CameraFile *new_file);
	int gp_image_flip_v (int file_number, CameraFile *new_file);


/* Utility functions for libraries
   ---------------------------------------------------------------- */
/* Image manipulation for in-memory images */
	int gp_file_image_rotate (CameraFile *old_file, int degrees, CameraFile *new_file);
	int gp_file_image_scale (CameraFile *old_file, int width, int height, CameraFile *new_file);
	int gp_file_image_flip_h (CameraFile *old_file, CameraFile *new_file);
	int gp_file_image_flip_v (CameraFile *old_file, CameraFile *new_file);

/* Displays the current status on an operation in the camera */
	int gp_update_status (char *status);

/* Displays a percentage done in the current operation */
	int gp_update_progress (float percentage);

/* Displays a message (error, very important info) */
	int gp_message (char *message);

/* Displays a configuration */
	int gp_confirm (char *message);
