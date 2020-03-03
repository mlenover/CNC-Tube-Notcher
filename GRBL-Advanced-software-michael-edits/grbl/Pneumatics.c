/*
  Pneumatics.c - Header file for shared definitions, variables, and functions
  Part of Grbl-Advanced

  Copyright (c) 2014-2016 Sungeun K. Jeon for Gnea Research LLC
  Copyright (c)	2017 Patrick F.
  Modified by Michael Lenover, 2020

  Grbl-Advanced is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl-Advanced is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl-Advanced.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "grbl_advance.h"
#include "GPIO.h"
#include "Pneumatics.h"


#define PRIMARY_GRIPPER     0
#define SECONDARY_GRIPPER   1
#define SHOT_PIN            2

void Pneumatics_Init(){
    GPIO_InitGPIO(GPIO_PNEUMATICS);
    Retract_Cylinder(PRIMARY_GRIPPER);
    Retract_Cylinder(SECONDARY_GRIPPER);
    Retract_Cylinder(SHOT_PIN);
}

void Extend_Cylinder(uint8_t cylNum){
    switch(cylNum){
        case 0:
            GPIO_SetBits(GPIO_PRIM_GRIP_PORT, GPIO_PRIM_GRIP_PIN);
            break;
        case 1:
            GPIO_SetBits(GPIO_SEC_GRIP_PORT, GPIO_SEC_GRIP_PIN);
            break;
        case 2:
            GPIO_SetBits(GPIO_SHOT_PIN_CYL_PORT, GPIO_SHOT_PIN_CYL_PIN);
            break;
    }
}

void Retract_Cylinder(uint8_t cylNum){
    switch(cylNum){
        case 0:
            GPIO_ResetBits(GPIO_PRIM_GRIP_PORT, GPIO_PRIM_GRIP_PIN);
            break;
        case 1:
            GPIO_ResetBits(GPIO_SEC_GRIP_PORT, GPIO_SEC_GRIP_PIN);
            break;
        case 2:
            GPIO_ResetBits(GPIO_SHOT_PIN_CYL_PORT, GPIO_SHOT_PIN_CYL_PIN);
            break;
    }
}

