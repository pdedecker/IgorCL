#include "XOPStandardHeaders.r"

resource 'vers' (1) {						// XOP version info.
	0x01, 0x20, release, 0x00, 0,			// version bytes and country integer.
	"1.20",
	"1.20, Copyright 1993-2010 WaveMetrics, Inc., all rights reserved."
};

resource 'vers' (2) {						// Igor version info.
	0x05, 0x00, release, 0x00, 0,			// Version bytes and country integer.
	"5.00",
	"(for Igor 5.00 or later)"
};

// Custom error messages
resource 'STR#' (1100) {
	{
		/* [1] */
		"Igor too old",
        /* [2] */
		"OpenCL returned an error (see command line output)",
	}
};

// No menu item

resource 'XOPI' (1100) {
	XOP_VERSION,							// XOP protocol version.
	DEV_SYS_CODE,							// Development system information.
	0,										// Obsolete - set to zero.
	0,										// Obsolete - set to zero.
	XOP_TOOLKIT_VERSION,					// XOP Toolkit version.
};

resource 'XOPC' (1100) {
	{
		"IgorCL",								// Name of operation.
		waveOP+XOPOp+compilableOp,			// Operation's category.
        
        "IgorCLCompile",					// Name of operation.
		waveOP+XOPOp+compilableOp,			// Operation's category.
        
        "IGORCLInfo",								// Name of operation.
		waveOP+XOPOp+compilableOp,			// Operation's category.
	}
};


