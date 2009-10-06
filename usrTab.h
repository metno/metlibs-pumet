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


#ifndef _usrTab_h
#define _usrTab_h

#include <puTools/miString.h>
#include <puTools/miTime.h>

#include <map>
#include <iostream>

#include <math.h>
#include <stdio.h>

using namespace std;

class usrTab {
private:
  map<miString, miString> entity;
  miString mNull;
public: 
  usrTab(): mNull("") {}

  bool create( miString );
 
  bool exists(miString m) const 
  { return ( bool(entity.count(m)) ? true: false);}

  miTime dates( miString );
 
  miString content( miString m) 
  { return ( bool(entity.count(m)) ? entity[m] : mNull); }

  miString replace( miString m) 
  { return ( bool(entity.count(m)) ? entity[m] : m);}

  miString file( miString m)
  { return entity["WORKDIR"]+ content(m); }
   
  void makeByHand(miString m, miString requ)
  { entity[m]= requ;}

  int numInt(miString m) 
    { return atoi(entity[m].cStr());}
  
  float numFloat( miString m)  
    { return atof(entity[m].cStr());}

  miString operator[] ( const miString &m )
  { return ( bool(entity.count(m)) ? entity[m] : mNull); }

  map<miString,miString> selected(const vector<miString> & );



};

#endif




