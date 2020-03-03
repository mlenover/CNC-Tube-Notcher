/*
  Pneumatics.h - Header file for shared definitions, variables, and functions
  Part of Grbl-Advanced

  Copyright (c) 2014-2016 Sungeun K. Jeon for Gnea Research LLC
  Copyright (c)	2017 Patrick F.
  Created by Michael Lenover, 2020

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
#ifndef PNEUMATICS_H
#define PNEUMATICS_H

//Initialize output pins
void Pneumatics_Init();

//Extend cylinder
void Extend_Cylinder(uint8_t cylNum);

//Retract cylinder
void Retract_Cylinder(uint8_t cylNum);

#endif // PNEUMATICS_H
