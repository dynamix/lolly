// "DigDug16x16" (GIF orig:5001 raw_payload:527 shrunk_payload:113 total:121 bytes)
// Compared to GIF: 2.42% 	Compared to raw: 22.96%
// using GIF index 0 as our transparency index 0 (blackened)
// ./heatshrink -w 8 -l 7 (background = 0; loop = 0; transparency = 0; )

FL_PROGMEM const struct HSpr_DigDug16x16 {
	uint16_t datasize = 113;
	uint16_t frames = 2;
	uint16_t duration = 250;
	uint8_t flags = 0;
	uint8_t palette_entries = 5;
	uint8_t crgb[0] = {
		// 0x00, 0x00, 0x00,  // original palette index 0
		// 0xcc, 0xcc, 0xcc,  // original palette index 1
		// 0x00, 0x66, 0xcc,  // original palette index 2
		// 0x00, 0x00, 0x00,  // original palette index 3
		// 0xff, 0x00, 0x00,  // original palette index 4
	};
	uint8_t hs_data[113] = {
		0x00, 0x02, 0xe6, 0x73, 0x39, 0x90, 0x0b, 0x37, 0x30, 0x10, 0x0b, 0xfe, 0x20, 0x2d, 0x01, 0x80, 
		0xc0, 0x41, 0xa3, 0x41, 0xe2, 0xc1, 0xc1, 0x02, 0x22, 0x41, 0xc1, 0xc2, 0x21, 0xc0, 0xe0, 0xa0, 
		0x40, 0x00, 0x50, 0x30, 0x08, 0x20, 0x79, 0x70, 0x80, 0x48, 0x10, 0x38, 0x28, 0x00, 0x14, 0x0a, 
		0x04, 0x3d, 0x0d, 0x04, 0x10, 0x05, 0x07, 0x86, 0x80, 0xc0, 0x20, 0x81, 0x00, 0x70, 0x48, 0x24, 
		0x10, 0x58, 0x0c, 0x0c, 0x08, 0x00, 0x0c, 0x82, 0x14, 0x1e, 0x08, 0x3e, 0x21, 0x1e, 0x28, 0x7e, 
		0x19, 0x0e, 0x0d, 0x60, 0x28, 0x36, 0x11, 0x84, 0x34, 0x00, 0x81, 0xe0, 0x45, 0xff, 0xc1, 0xf4, 
		0x11, 0xfe, 0x4c, 0x7e, 0x14, 0x3e, 0x21, 0x3e, 0x28, 0x1c, 0x15, 0xfa, 0x34, 0x42, 0x21, 0xd2, 
		0x58, 
	};
} HSpr_DigDug16x16;