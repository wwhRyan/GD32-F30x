/******************************************************************************
* (c) COPYRIGHT 2017 RAONTECH, Inc. ALL RIGHTS RESERVED.                      *
*                                                                             *
* This software is the property of RAONTECH and is furnished under license    *
* by RAONTECH.                                                                *
* This software may be used only in accordance with the terms of said license.*
* This copyright noitce may not be removed, modified or obliterated without   *
* the prior written permission of RAONTECH, Inc.                              *
*                                                                             *
* This software may not be copied, transmitted, provided to or otherwise      *
* made available to any other person, company, corporation or other entity    *
* except as specified in the terms of said license.                           *
*                                                                             *
* No right, title, ownership or other interest in the software is hereby      *
* granted or transferred.                                                     *
*                                                                             *
* The information contained herein is subject to change without notice        *
* and should not be construed as a commitment by RAONTECH, Inc.               *
******************************************************************************/

#include "rti_vc_api.h"
#include "i2c.h"
#if defined(__APPLE__)
#include <unistd.h>
#endif

void rtiVC_DelayUs(U32_T microseconds)
{
	// Porting here!

#if (!defined(__KERNEL__) && defined(__linux__) /* Linux application */ || defined(__APPLE__) )
	usleep(microseconds);
	
#elif defined(_WIN32)
	/* NOTE: Windows can't sleep for less than a millisecond. */
	DWORD cnt;

	cnt = DIV_ROUND_UP(microseconds, 1000);
	Sleep(cnt);
	
#else
	DelayUs(microseconds);	
#endif
}

