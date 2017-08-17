/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2017 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "../../SDL_internal.h"

#if SDL_VIDEO_DRIVER_VITA

#include <psp2/kernel/processmgr.h>
#include <psp2/touch.h>

#include "SDL_events.h"
#include "SDL_log.h"
#include "SDL_vitavideo.h"
#include "SDL_vitatouch.h"
#include "../../events/SDL_mouse_c.h"
#include "../../events/SDL_touch_c.h"

SceTouchData touch_old[SCE_TOUCH_PORT_MAX_NUM];
SceTouchData touch[SCE_TOUCH_PORT_MAX_NUM];

void 
VITA_InitTouch(void)
{
	sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
	sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, SCE_TOUCH_SAMPLING_STATE_START);
	sceTouchEnableTouchForce(SCE_TOUCH_PORT_FRONT);
	sceTouchEnableTouchForce(SCE_TOUCH_PORT_BACK);

	// Support passing both front and back touch devices in events
	SDL_AddTouch((SDL_TouchID)0, "Front");
	SDL_AddTouch((SDL_TouchID)1, "Back");
}

void 
VITA_QuitTouch(void){
	sceTouchDisableTouchForce(SCE_TOUCH_PORT_FRONT);
	sceTouchDisableTouchForce(SCE_TOUCH_PORT_BACK);
}

void 
VITA_PollTouch(void)
{
	// We skip polling touch if no window is created
	if (Vita_Window == NULL)
		return;

	SDL_FingerID finger_id = 0;
	int port;

	memcpy(touch_old, touch, sizeof(touch_old));

	for(port = 0; port < SCE_TOUCH_PORT_MAX_NUM; port++) {
		sceTouchPeek(port, &touch[port], 1);
		if (touch[port].reportNum > 0)
		{
			// Send an initial touch
			SDL_SendTouch((SDL_TouchID)port, 
				finger_id, 
				SDL_TRUE, 
				touch[port].report[0].x, 
				touch[port].report[0].y, 
				touch[port].report[0].force);

			// Always send the motion
			SDL_SendTouchMotion((SDL_TouchID)port, 
				finger_id,
				touch[port].report[0].x,
				touch[port].report[0].y,
				touch[port].report[0].force);
		}
		else if (touch_old[port].reportNum > 0)
		{
			// Finger released from screen
			SDL_SendTouch((SDL_TouchID)port,
				finger_id,
				SDL_FALSE,
				touch[port].report[0].x,
				touch[port].report[0].y,
				touch[port].report[0].force);
		}
	}
}

#endif /* SDL_VIDEO_DRIVER_VITA */

/* vi: set ts=4 sw=4 expandtab: */
