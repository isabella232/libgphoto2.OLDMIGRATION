#ifndef __GSMART_AVI_HEADER_H
#define __GSMART_AVI_HEADER_H

#define GSMART_AVI_FRAME_HEADER_LENGTH	8
u_int8_t GsmartAviFrameHeader[GSMART_AVI_FRAME_HEADER_LENGTH] = {   
	0x30, 0x30, 0x64, 0x63, // 00dc
	0x00, 0x00, 0x00, 0x00 // size of frame
};

#define GSMART_AVI_HEADER_LENGTH 224
u_int8_t GsmartAviHeader[GSMART_AVI_HEADER_LENGTH] = {   
	/* -- RIFF HEADER -- */
	0x52, 0x49, 0x46, 0x46, // RIFF  
	0x00, 0x00, 0x00, 0x00, // number of bytes following
	0x41, 0x56, 0x49, 0x20, // AVI
	0x4C, 0x49, 0x53, 0x54, // LIST
	0xC0, 0x00, 0x00, 0x00, // length of list in bytes
	0x68, 0x64, 0x72, 0x6C, // hdrl
	0x61, 0x76, 0x69, 0x68, // avih
	0x38, 0x00, 0x00, 0x00, // number of bytes to follow
	0xb5, 0x8c, 0x01, 0x00, // microseconds per frame
	0x00, 0x00, 0x06, 0x00, // max bytes per second
	0x00, 0x00, 0x00, 0x00, // PaddingGranularity (whatever that might be)
	                        // Other sources call it 'reserved'
	0x00, 0x00, 0x01, 0x00, // flags, only  WASCAPTUREFILE, no index
	0x00, 0x00, 0x00, 0x00, // total frames
	0x00, 0x00, 0x00, 0x00, // initial frames
	0x01, 0x00, 0x00, 0x00, // number of streams
	0x00, 0x00, 0x00, 0x00, // suggested buffer size
	0x40, 0x01, 0x00, 0x00, // width
	0xf0, 0x00, 0x00, 0x00, // height
	0x00, 0x00, 0x00, 0x00, // some reserved long values
	0x00, 0x00, 0x00, 0x00, //
	0x00, 0x00, 0x00, 0x00, //
	0x00, 0x00, 0x00, 0x00, //
	/* -- video stream list -- */
	0x4C, 0x49, 0x53, 0x54, // LIST
	0x74, 0x00, 0x00, 0x00, // length of list in bytes
	0x73, 0x74, 0x72, 0x6C, // strl
	/* -- video stream header -- */
	0x73, 0x74, 0x72, 0x68, // strh
	0x38, 0x00, 0x00, 0x00, // # of bytes to follow
	0x76, 0x69, 0x64, 0x73, // vids
	0x4D, 0x4A, 0x50, 0x47, // MJPG
	0x00, 0x00, 0x00, 0x00, // some more reserved longs
	0x00, 0x00, 0x00, 0x00, //
	0x00, 0x00, 0x00, 0x00, //
	0x33, 0x18, 0x00, 0x00, // ms_per_frame scale
	0x48, 0xEE, 0x00, 0x00, // Rate: Rate/Scale == samples/second 
	0x00, 0x00, 0x00, 0x00, // start
	0x00, 0x00, 0x00, 0x00, // length (video_frames)
	0x00, 0x00, 0x00, 0x00, // suggested buffer size
	0xFF, 0xFF, 0xFF, 0xFF, // quality
	0x00, 0x00, 0x00, 0x00, // sample size
	0x00, 0x00, 0x00, 0x00, // frame
	0x00, 0x00, 0x00, 0x00, // frame
	/* -- video stream format -- */
	0x73, 0x74, 0x72, 0x66, // strf
	0x28, 0x00, 0x00, 0x00, // number of bytes to follow (40)
	0x28, 0x00, 0x00, 0x00, // size (dito)
	0x40, 0x01, 0x00, 0x00, // width
	0xf0, 0x00, 0x00, 0x00, // height
	0x01, 0x00, 0x0C, 0x00, // planes(1) : count(24)
	0x4D, 0x4A, 0x50, 0x47, // encoder MJPG
	0x00, 0xC2, 0x01, 0x00, // Size image
	0x00, 0x00, 0x00, 0x00, // XPelsPerMeter
	0x00, 0x00, 0x00, 0x00, // YPelsPerMeter
	0x00, 0x00, 0x00, 0x00, // Number of colors used
	0x00, 0x00, 0x00, 0x00, // Number of colors important
	/* do we have to pad with JUNK here ? */
	0x4C, 0x49, 0x53, 0x54, // LIST
	0x00, 0x00, 0x00, 0x00, // size of list
	0x6D, 0x6F, 0x76, 0x69  // movi
};

#endif /* __GSMART_AVI_HEADER_H */
