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
      lli(661),
      agr(170),
      fogi(665),
      myerror(errorSymbol) {}

  bool initializeModel(vector<paramet>);
  static void readSymbols(miutil::miString);
  void periods(vector<miutil::miTime>,int,int,bool compute_minmax = false);

  /**
   * This function creates a symbol that is not adjusted with repect on
   * temperature (state of aggregate). The symbol can be adjusted with a
   * call to stateMaker( miSymbol &symbol_, float temperature,
	*	                   float stateOfAggregate,
	*                     float lightningIndex,
	*                     float fogIndex )
	*/
  vector<miSymbol> computeWithoutStateOfAggregate( const vector<paramet> &AllModelData,
  		                                             const vector<miutil::miTime> &termin,
  				      				                     int min = 3,int max = 6);


  vector<miSymbol> compute(vector<paramet>, vector<miutil::miTime>,int,int);
  vector<miSymbol> compute_new(vector<paramet>, vector<miutil::miTime>,
			       int,int,bool compute_minmax = false);
  vector<float>    water_state(vector<float>);


  static miSymbol getErrorSymbol() { return errorSymbol; }

  bool cloudMaker(miutil::miTime);
  bool rainMaker(miutil::miTime);
  bool tempMaker(miutil::miTime);


  /**
   * Create an state of aggregate from temperaure.
   * The teperature is in degree Celsius.
   *
   *  0 (snow) temperature <= 0.5.
   *  1 (sleet) 0.5 < temperature < 1.5.
   *  2 (rain)  temeperature >= 1.5
   */
  static int stateOfAggregateFromTemperature( float temperature );



  /**
   * Change the state of aggregate of the symbol based on the temperature.
   *
   * The change is as follow, only symbols with precipitation is in the table.
   * Other symbols is not affected.
   *
   * The state is either form the model or we can compute it from
   * the temperature. Se the function stateOfAggregateFromTemperature( float temperature ).
   *
   * +--------------+-----------------------------------+
   * |              |            state                  |
   * +--------------+---------+----------+--------------+
   * | SYMBOL       |    0    |    1     |     2        |
   * +--------------+---------+----------+--------------+
   * | LIGHTRAINSUN | SNOWSUN | SLEETSUN |     -        |
   * | LIGHTRAIN    | SNOW    | SLEET    |     -        |
   * | RAIN         | SNOW    | SLEET    |     -        |
   * | SNOW         |   -     | SLEET    | RAIN         |
   * | SNOWSUN      |   -     | SLEETSUN | LIGHTRAINSUN |
   * | SLEET        | SNOW    |    -     | RAIN         |
   * | SLEETSUN     | SNOWSUN |    -     | LIGHTRAINSUN |
   * +--------------+---------+----------+--------------+
   *
   * If there is lightning the symbol generated fom the table above
   * vil be adjusted as follow.
   *
   *
   * LIGTHRAINSUN -->  LIGHTRAINTHUNDERSUN
   * LIGTHRAIN    -->  LIGHTRAINTHUNDERSUN
   * SLEETSUN     -->  LIGHTRAINTHUNDERSUN
   * RAIN         -->  RAINNTHUNDER
   * SNOW         -->  SNOWTHUNDER
   * SNOWSUN      -->  SNOWTHUNDER
   * SLEET        -->  SNOWTHUNDER
   *
   * If there is fog, it will override all other symbols and the symbol is set to FOG.
   */
  static bool stateMaker( miSymbol &symbol_,
	                       float temperature,
		                    float stateOfAggregate = FLT_MAX,
	                       float lightningIndex = FLT_MAX,
	                       float fogIndex = FLT_MAX );



  bool stateMaker( miutil::miTime );

  /**
   * If there is lightning add lightning to the symbolg. The lightningIndex
   * is a true/false value. Where 0 is false and lightningIndex != 0 is true.
   *
   * If fogIndex is FLT_MAX or FLT_MIN the function does nothing. It is
   * treated undefined values.
   */

  static void lightningMaker( miSymbol &symbol_,
		                        float lightningIndex );

  bool lightningMaker(miutil::miTime);

  /**
   * If there is fog set the symbol to fog. The fogIndex
   * is a true/false value. Where 0 is false and fogIndex != 0 is true.
   *
   * If fogIndex is FLT_MAX or FLT_MIN the function does nothing. It is
   * treated undefined values.
   */
  static void fogMaker( miSymbol &symbol_, float fogIndex);

  bool fogMaker(miutil::miTime);

  bool signChange(int, int);
  miSymbol getSymbol(int );

  bool isShower(int);
  bool isPrecip(int);
  bool isDry(int);

  static int maxCustom() { return MAXcustom;}
  static int minCustom() { return MINcustom;}
  void make_pos_symbols(map<int,int>&, map<int,miutil::miString>& );
};

#endif


