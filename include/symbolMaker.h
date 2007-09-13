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



#ifndef _symbolMaker_h
#define _symbolMaker_h

#include "miSymbol.h"
#include "paramet.h"

#include <puTools/miString.h>
#include <puTools/miTime.h>
#include <puTools/puMathAlgo.h>

#include <vector>
#include <iostream>
#include <algorithm>
#include <map>

#include <stdio.h>
#include <math.h>




// this program is used to compute weather symbols from 
// cloud, precip and temperature data.
// The symbols er created by a file, which can be edited.
// It is possible to change the customer numbers, or the language
// of the used text. The standart file is:
// /work/pu/input/weatherSymbols.nor
// the program checks dynamically the 6 hours before and after
// the requested termin for data. 
// Because of that it is compatible with ECMWF 6-hours prognosis
// as well as HIRLAM 3-hours forecasts.
// JS/PU 7-98
// last modified 9-98 added vis output
// and the pos. to get a symbol by customNumber


// initializes by construction the ident numbers of the
// requested parameters, sorts the model input and does the job

using namespace std;

class symbolMaker{
  friend class cloudGrp;
private:
  //int MAXcustom, MINcustom;
  float latitude;

  miSymbol symbol; //  rawdata ...
  paramet t2m ;    // temperature in 2 m
  paramet rrf ;    // frontal precip. RR6h, average (t-6h, t+6h)
  paramet rrk ;    // convect. precip.        - " -
  paramet rrt ;    // total precip.           - " -
  paramet cs  ;    // cloud amount %  surface level (1000 hPa)
  paramet cl  ;    //     - " -       low level     (850  hPa)
  paramet cm  ;    //     - " -       middel level  (500  hPa)
  paramet ch  ;    //     - " -       high  level   (300  hPa)
  paramet ct  ;    //     - " -       total
  paramet rrtotal; //  averaged data ...
  paramet csurf;
  paramet clow;
  paramet cmiddle;
  paramet cheight;
  paramet ctotal;
  paramet lli;      // lightning index ( 0 / 1 )
  paramet agr;      // aggregation (0 = water;1 = sleet;2 = snow)    
  paramet fogi;     // fog-indicator (0 / 1)
  paramet periodOfRain;
  paramet periodOfNoRain;


  
  static int MAXcustom;
  static int MINcustom;	
  static miSymbol sun;                           
  static miSymbol lightCloud;                      
  static miSymbol partlyCloud;                     
  static miSymbol cloud;
  static miSymbol fog;
  static miSymbol lightRainSun;
  static miSymbol lightRain;
  static miSymbol rain;
  static miSymbol sleetSun;
  static miSymbol snowSun;
  static miSymbol sleet;
  static miSymbol snow;
  static miSymbol lightRainThunderSun;
  static miSymbol RainThunder;
  static miSymbol SnowThunder;
  static miSymbol errorSymbol;
  
  miSymbol myerror;
public:
  symbolMaker()
    : latitude(60),
      t2m(31),
      rrf(19), 
      rrk(20),
      rrt(17),
      cs(3901),
      cl(3902),
      cm(3903),
      ch(3904),
      ct(25),
      rrtotal(17),
      csurf(3901),
      clow(3902),
      cmiddle(3903),
      cheight(3904),
      ctotal(25),
      agr(170),
      fogi(665),
      lli(661),
      myerror(errorSymbol) {}
  
  bool initializeModel(vector<paramet>);
  static void readSymbols(miString);
  void periods(vector<miTime>,int,int,bool compute_minmax = false);

  vector<miSymbol> compute(vector<paramet>, vector<miTime>,int,int);
  vector<miSymbol> compute_new(vector<paramet>, vector<miTime>,
			       int,int,bool compute_minmax = false);
  vector<float>    water_state(vector<float>);


  static miSymbol getErrorSymbol() { return errorSymbol; }

  bool cloudMaker(miTime);
  bool rainMaker(miTime);
  bool tempMaker(miTime);
  bool stateMaker(miTime);
  bool lightningMaker(miTime);
  bool fogMaker(miTime);

  bool signChange(int, int);
  miSymbol getSymbol(int );

  bool isShower(int);
  bool isPrecip(int);
  bool isDry(int);
  
  static int maxCustom() { return MAXcustom;}
  static int minCustom() { return MINcustom;}
  void make_pos_symbols(map<int,int>&, map<int,miString>& );
};

#endif


