/*
  libpuMet - Meteorological algorithms (Weather symbol generator etc.)
  
  $Id$

  Copyright (C) 2006 met.no

  Contact information:
  Norwegian Meteorological Institute
  Box 43 Blindern
  0313 OSLO
  NORWAY
  email: diana@met.no
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef _iceAccretion_h
#define _iceAccretion_h

// Prgmmr: D. M. Feit   org: W/NMC21    date: 87-06-26
// Program history log:                                                  
//   87-06-26  D. M. Feit
//   96-07-23  L. D. Burroughs - converted for use on cray
// 2006-07-13  L.Bergolt - C++                                        
//

/**

  \brief  Rate of ice accretion in cm/hr

*/

double iceAccretion(const double& ta,   //air temperature in deg celcius
		    const double& tw,   //sea surface temperature in deg celcius
		    const double& wspd); //wind speed in meters/sec



#endif
