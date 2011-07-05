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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <fstream>
#include "symbolMaker.h"

using namespace std;
using namespace miutil;
using namespace puMathAlgo;

#define SLOG( logmsg ) { \
   if( slog ) { \
      *slog << logmsg; \
   } \
}

// initializes the symbols with an
// internal number and the WMO-code
const float DUMMY = -999;

int symbolMaker::MAXcustom = -1000;
int symbolMaker::MINcustom = 1000;
miSymbol symbolMaker::Sun(1, false);
miSymbol symbolMaker::LightCloud(2, false);
miSymbol symbolMaker::PartlyCloud(3, false);
miSymbol symbolMaker::Cloud(4, false);
miSymbol symbolMaker::Fog(5, true);
miSymbol symbolMaker::LightRainSun(10, true);
miSymbol symbolMaker::LightRain(20, true);
miSymbol symbolMaker::Rain(30, true);
miSymbol symbolMaker::SleetSun(100, true);
miSymbol symbolMaker::SnowSun(200, true);
miSymbol symbolMaker::Sleet(300, true);
miSymbol symbolMaker::Snow(400, true);
miSymbol symbolMaker::LightRainThunderSun(1000, true);
miSymbol symbolMaker::RainThunder(2000, true);
miSymbol symbolMaker::SnowThunder(3000, true);
miSymbol symbolMaker::SleetSunThunder(4000, true);
miSymbol symbolMaker::SnowSunThunder(5000, true);
miSymbol symbolMaker::LightRainThunder( 6000, true );
miSymbol symbolMaker::SleetThunder(7000, true);
miSymbol symbolMaker::ErrorSymbol(999, true);

static bool fogstatus = true;
static bool thunderstatus = false;
static bool visiblestatus = false;
static bool picturestatus = false;
static bool longterm = false;
static bool useDynamicRainLimits=false;
//float latitude = 60;


// reads symbol data from file, and builds the classes for symbols out of it

void symbolMaker::readSymbols(miString SymbolFname)
{
  MAXcustom = -1000;
  MINcustom = 1000;

  code normal;
  code darkness;

  ifstream codeFile;
  miString readMi;
  miString darkName, normalName, className;
  miString darkPic, normalPic;
  int darkIndex, normalIndex;
  int Vision;
  vector<miString> words;

  // reading begins here

  codeFile.open(SymbolFname.cStr(), ios::in);

  if (!codeFile)
    cerr << "SymbolMaker error: File " << SymbolFname << " not found" << endl;

  while (codeFile.good()) {

    getline(codeFile, readMi);
    readMi.trim();
    if (readMi.contains("#"))
      continue;

    // flags

    if (readMi.contains("FOGSTATUS"))
      if (readMi.contains("false"))
        fogstatus = false;

    if (readMi.contains("THUNDERSTATUS"))
       if (readMi.contains("true"))
          thunderstatus = true;

    if (readMi.contains("VISIBILITY"))
      if (readMi.contains("true"))
        visiblestatus = true;

    if (readMi.contains("PICTURES"))
      if (readMi.contains("true"))
        picturestatus = true;

    if (readMi.contains("LONGTERM"))
      if (readMi.contains("true"))
        longterm = true;

    if (readMi.contains("DYNAMIC_RAINLIMITS"))
       if (readMi.contains("true"))
          useDynamicRainLimits = true;


    if (readMi == "{") {
      getline(codeFile, className);
      className.trim();
      getline(codeFile, normalName);
      words = normalName.split();
      normalName.replace(words[0], "");
      normalIndex = atoi(words[0].cStr());
      normalName.trim();
      if (picturestatus) {
        getline(codeFile, normalPic);
        normalPic.trim();
      }
      getline(codeFile, darkName);
      darkName.trim();
      words = darkName.split();
      darkName.replace(words[0], "");
      darkIndex = atoi(words[0].cStr());
      darkName.trim();
      if (picturestatus) {
        getline(codeFile, darkPic);
        darkPic.trim();
      }

      // here starts the input of the symboldependet
      // first guess for the vision
      if (visiblestatus)
        codeFile >> Vision;
      else
        Vision = 9;

      if (darkIndex > MAXcustom)
        MAXcustom = darkIndex; // 2000-11-17: normalIndex here!!

      if (normalIndex < MINcustom)
        MINcustom = normalIndex;

      normal.AddValues(normalIndex, normalName, Vision, normalPic);
      darkness.AddValues(darkIndex, darkName, Vision, darkPic);

      // and here it ends
      // compares and sorts the readed data into  the symbols

      if (className == "SUN")
        Sun.AddCodes(normal, darkness);

      if (className == "LIGHTCLOUD")
        LightCloud.AddCodes(normal, darkness);

      if (className == "PARTLYCLOUD")
        PartlyCloud.AddCodes(normal, darkness);

      if (className == "CLOUD")
        Cloud.AddCodes(normal, darkness);

      if (className == "FOG")
        Fog.AddCodes(normal, darkness);

      if (className == "LIGHTRAINSUN")
        LightRainSun.AddCodes(normal, darkness);

      if (className == "LIGHTRAIN")
        LightRain.AddCodes(normal, darkness);

      if (className == "RAIN")
        Rain.AddCodes(normal, darkness);

      if (className == "SLEETSUN")
        SleetSun.AddCodes(normal, darkness);

      if (className == "SNOWSUN")
        SnowSun.AddCodes(normal, darkness);

      if (className == "SLEET")
        Sleet.AddCodes(normal, darkness);

      if (className == "SNOW")
        Snow.AddCodes(normal, darkness);

      if (className == "LIGHTRAINTHUNDERSUN")
        LightRainThunderSun.AddCodes(normal, darkness);

      if (className == "RAINTHUNDER")
        RainThunder.AddCodes(normal, darkness);

      if (className == "SNOWTHUNDER")
        SnowThunder.AddCodes(normal, darkness);

      if (className == "SLEETSUNTHUNDER")
         SleetSunThunder.AddCodes(normal, darkness);

      if (className == "SNOWSUNTHUNDER")
         SnowSunThunder.AddCodes(normal, darkness);

      if (className == "LIGHTRAINTHUNDER")
         LightRainThunder.AddCodes(normal, darkness);

      if (className == "SLEETTHUNDER")
         SleetThunder.AddCodes(normal, darkness);

      if (className == "ERROR")
        ErrorSymbol.AddCodes(normal, darkness);

    }
  }
  codeFile.close();
}
;

// sorts the input parameters into the given classes by ident number

bool symbolMaker::initializeModel(vector<paramet> AllData)
{
  t2m.erase();
  rrf.erase();
  rrk.erase();
  rrt.erase();
  cs.erase();
  cl.erase();
  cm.erase();
  ch.erase();
  ct.erase();
  agr.erase();
  lli.erase();
  fogi.erase();

  for ( vector<paramet>::size_type i = 0; i < AllData.size(); i++) {

    if (AllData[i].paraNO == t2m.paraNO)
      t2m = AllData[i];

    if (AllData[i].paraNO == rrf.paraNO)
      rrf = AllData[i];

    if (AllData[i].paraNO == rrk.paraNO)
      rrk = AllData[i];

    if (AllData[i].paraNO == rrt.paraNO)
      rrt = AllData[i];

    if (AllData[i].paraNO == cs.paraNO)
      cs = AllData[i];

    if (AllData[i].paraNO == cl.paraNO)
      cl = AllData[i];

    if (AllData[i].paraNO == cm.paraNO)
      cm = AllData[i];

    if (AllData[i].paraNO == ch.paraNO)
      ch = AllData[i];

    if (AllData[i].paraNO == ct.paraNO)
      ct = AllData[i];

    if (AllData[i].paraNO == agr.paraNO)
      agr = AllData[i];

    if (AllData[i].paraNO == lli.paraNO)
      lli = AllData[i];

    if (AllData[i].paraNO == fogi.paraNO)
      fogi = AllData[i];

  }

  return (true);
}
;

// checks out for periods of rain and computes
// averaged values of the prec. and clouds of the 6 or 12 hours
// round the forecast termin. (dependet on the input)

void symbolMaker::periods(vector<miTime> termin, int min = 3, int max = 6,
    bool compute_minmax)
{

  average<float> avRR(DUMMY);
  average<float> avCL(DUMMY);
  average<float> avCS(DUMMY);
  average<float> avCM(DUMMY);
  average<float> avCH(DUMMY);
  average<float> avCT(DUMMY);

  miTime runTerm;
  miTime stopTerm;
  float rrto, rrtot, rrconv, rrfront;
  float nrr1, nrr0;

  periodOfRain.erase();
  periodOfNoRain.erase();
  rrtotal.erase();
  csurf.erase();
  clow.erase();
  cmiddle.erase();
  cheight.erase();
  ctotal.erase();

  for (vector<miTime>::size_type i = 0; i < termin.size(); i++) {
    nrr0 = 0;
    nrr1 = 0;
    rrto = DUMMY;
    rrtot = DUMMY;

    // compute new min/max
    if (compute_minmax) {

      if (i == 0) {
        runTerm = termin[i];
        runTerm.addHour(-min + 1);
        stopTerm = termin[i];
        stopTerm.addHour(max);
      } else {
        runTerm = stopTerm;
        int diff;
        if (i < termin.size() - 1) { // not the last timepoint
          // rhs cover half the interval to next timepoint
          diff = miTime::hourDiff(termin[i + 1], termin[i]) / 2;
        } else {
          // last timepoint; rhs equal to lhs
          diff = miTime::hourDiff(termin[i], runTerm);
        }
        stopTerm = termin[i];
        stopTerm.addHour(diff);
        runTerm.addHour(1);
      }
      //       cerr << "--runTerm:" << runTerm
      // 	   << " termin:" << termin[i]
      // 	   << " stopTerm:" << stopTerm << endl;

    } else {

      runTerm = termin[i];
      stopTerm = termin[i];

      runTerm.addHour(-min);
      stopTerm.addHour(max);
    }

    while (runTerm <= stopTerm) {
      rrtot = rrt.value(runTerm);
      latitude = rrt.lat();

      if (rrtot == DUMMY) {
        rrconv = rrk.value(runTerm);
        rrfront = rrf.value(runTerm);

        if (rrconv != DUMMY && rrfront == DUMMY) {
          rrtot = rrconv;
          latitude = rrk.lat();
        } else if (rrconv == DUMMY && rrfront != DUMMY) {
          rrtot = rrfront;
          latitude = rrf.lat();
        } else if (rrconv != DUMMY && rrfront != DUMMY)
          rrtot = rrfront + rrconv;
      }

      if (rrtot != DUMMY) {
        if (rrto == DUMMY)
          rrto = 0;
        rrto += rrtot;
      }

      if (runTerm == termin[i] || runTerm == stopTerm) {
        if (rrto > rainLimit /*0.6*/)
          nrr1++;
        else if (rrto < noRainLimit /*0.2*/ && rrto > DUMMY)
          nrr0++;
        avRR.add(rrto);
        rrto = DUMMY;
      }

      avCL.add(cl.value(runTerm));
      avCS.add(cs.value(runTerm));
      avCM.add(cm.value(runTerm));
      avCH.add(ch.value(runTerm));
      avCT.add(ct.value(runTerm));

      runTerm.addHour(1);
    }

    periodOfRain.push(termin[i], nrr1);
    periodOfNoRain.push(termin[i], nrr0);
    rrtotal.push(termin[i], avRR.middle());
    csurf.push(termin[i], avCS.middle());
    clow.push(termin[i], avCL.middle());
    cmiddle.push(termin[i], avCM.middle());
    cheight.push(termin[i], avCH.middle());
    ctotal.push(termin[i], avCT.middle());
  }
}
;

// checks out for clouds > 1st step

bool symbolMaker::cloudMaker(miTime now)
{
  myerror.AddErr("DATA ERROR > NO CLOUD DATA");
  symbol = myerror;

  if (ctotal.value(now) != DUMMY)
    symbol = Sun;

  if (clow.value(now) != DUMMY && cmiddle.value(now) != DUMMY && cheight.value(
      now) != DUMMY && csurf.value(now) != DUMMY)
    symbol = Sun;

  if (symbol == myerror)
    return (false);

  if (ctotal.value(now) >= 13)
    symbol = LightCloud;

  if (ctotal.value(now) > 38)
    symbol = PartlyCloud;

  if (ctotal.value(now) > 86)
    symbol = Cloud;

  if (csurf.value(now) > 15 || clow.value(now) > 15 || cmiddle.value(now) > 15
      || cheight.value(now) > 25)
    symbol = LightCloud;

  if (csurf.value(now) > 45 || clow.value(now) > 45 || cmiddle.value(now) > 45
      || cheight.value(now) > 55)
    symbol = PartlyCloud;

  if (csurf.value(now) > 86 || clow.value(now) > 86 || cmiddle.value(now) > 86
      || cheight.value(now) > 91)
    symbol = Cloud;

  //  if(fogstatus && csurf.value(now) > 75 )
  //     symbol = fog;

  return (true);
}
;

// checks out for precipitation >  2nd step

bool symbolMaker::rainMaker(miTime now)
{
  myerror.AddErr("DATA ERROR > NO PRECIPITATION DATA");

  float rrNow = rrtotal.value(now);
  float pRain = periodOfRain.value(now);
  float noRain = periodOfNoRain.value(now);

  if (rrtotal.value(now) == DUMMY) {
    symbol = myerror;
    return (false);
  }

  if (longterm)
    rrNow /= 2;

  // changes: 2002-08-21
  // range 0.1 -> 0.2
  // range 0.3 -> 0.6


  if (rrNow > noRainLimit /*0.2*/ || pRain > 0) {

    if (symbol == Sun || symbol == LightCloud)
      symbol = LightRainSun;

    if (symbol == PartlyCloud) {
      if (rrNow <= rainLimit /*0.6*/ || noRain >= pRain)
        symbol = LightRainSun;
      else
        symbol = LightRain;
    }

    if (symbol == Cloud && rrNow > noRainLimit /*0.2*/)
      symbol = LightRain;

    if (symbol == LightRain && rrNow > 5)
      symbol = Rain;
  }

  return true;
}


// checks out the weather relevant temperatures (checks for snow etc.)
// only relevant if precipitation occurs > 3rd step

bool symbolMaker::tempMaker(miTime now)
{

  float temperature;

  myerror.AddErr("DATA ERROR > NO TEMPERATURE DATA");

  temperature = t2m.value(now);

  if (temperature == DUMMY) {
    if (symbol == LightRainSun || symbol == LightRain || symbol == Rain) {
      symbol = myerror;
      return (false);
    }
  }

  if (symbol == LightRainSun) {
    if (temperature <= 0.5)
      symbol = SnowSun;
    if (temperature > 0.5 && temperature < 1.5)
      symbol = SleetSun;
  }

  if (symbol == LightRain || symbol == Rain) {
    if (temperature <= 0.5)
      symbol = Snow;
    if (temperature > 0.5 && temperature < 1.5)
      symbol = Sleet;
  }
  return (true);
}


// main program


vector<miSymbol> symbolMaker::computeWithoutStateOfAggregate(const vector<
    paramet> &AllModelData, const vector<miTime> &termin, int min, int max,
    bool useNewLightningMaker )
{
  vector<miSymbol> tmpSymbols;

  if( ! useDynamicRainLimits ) {
     noRainLimit = 0.2;
     rainLimit = 0.6;
  } else {
     int times=min+max-1;
     noRainLimit = 0.2 + 0.05*(times<0?0:times);
     rainLimit = noRainLimit + 0.4;
     SLOG( "symbolMaker::compute: min: " << min << " max: " << max << " periods: " << (min+max-1)
            << " noRainLimit: " << noRainLimit << " rainLimit: " << rainLimit );
  }

  myerror = ErrorSymbol;
  initializeModel(AllModelData);
  periods(termin, min, max);

  for (vector<miTime>::size_type yet = 0; yet < termin.size(); yet++) {
    cloudMaker(termin[yet]);
    if (symbol != myerror)
      rainMaker(termin[yet]);

    if( thunderstatus && symbol != myerror ) {
       if( useNewLightningMaker )
          lightningMakerNew( symbol, lli.value( termin[yet] ) );
       else
          lightningMaker( symbol, lli.value( termin[yet] ) );
    }

    if( fogstatus && symbol != myerror )
       fogMaker( symbol, fogi.value( termin[yet] ) );

    symbol.setTime(termin[yet]);
    symbol.setLightStat(termin[yet], latitude);
    tmpSymbols.push_back(symbol);
  }

  return tmpSymbols;

}
;

vector<miSymbol> symbolMaker::compute(vector<paramet> AllModelData, vector<
    miTime> termin, int min, int max)
{
  vector<miSymbol> tmpSymbols;

  if( ! useDynamicRainLimits ) {
     noRainLimit = 0.2;
     rainLimit = 0.6;
  } else {
     int times=min+max-1;
     noRainLimit = 0.2 + 0.05*(times<0?0:times);
     rainLimit = noRainLimit + 0.4; //diff: 0.6 - 0.2
     SLOG( "symbolMaker::compute: min: " << min << " max: " << max << " periods: " << (min+max-1)
           << " noRainLimit: " << noRainLimit << " rainLimit: " << rainLimit );
  }

  myerror = ErrorSymbol;
  initializeModel(AllModelData);
  periods(termin, min, max);
  for (vector<miTime>::size_type yet = 0; yet < termin.size(); yet++) {
    cloudMaker(termin[yet]);
    if (symbol != myerror)
      rainMaker(termin[yet]);
    if (symbol != myerror)
      tempMaker(termin[yet]);

    symbol.setTime(termin[yet]);
    symbol.setLightStat(termin[yet], latitude);
    tmpSymbols.push_back(symbol);
  }
  return (tmpSymbols);

}
;

miSymbol symbolMaker::getSymbol(int Cnumber)
{

  miSymbol tmp;

  for (int testlight = 1; testlight >= 0; testlight--) {

    if (Cnumber == Sun.customNumber(testlight)) {
      tmp = Sun;
      tmp.setLightStat(testlight);
      return tmp;
    }
    if (Cnumber == LightCloud.customNumber(testlight)) {
      tmp = LightCloud;
      tmp.setLightStat(testlight);
      return tmp;
    }
    if (Cnumber == PartlyCloud.customNumber(testlight)) {
      tmp = PartlyCloud;
      tmp.setLightStat(testlight);
      return tmp;
    }
    if (Cnumber == Cloud.customNumber(testlight)) {
      tmp = Cloud;
      tmp.setLightStat(testlight);
      return tmp;
    }
    if (Cnumber == Fog.customNumber(testlight)) {
      tmp = Fog;
      tmp.setLightStat(testlight);
      return tmp;
    }
    if (Cnumber == LightRainSun.customNumber(testlight)) {
      tmp = LightRainSun;
      tmp.setLightStat(testlight);
      return tmp;
    }
    if (Cnumber == LightRain.customNumber(testlight)) {
      tmp = LightRain;
      tmp.setLightStat(testlight);
      return tmp;
    }
    if (Cnumber == Rain.customNumber(testlight)) {
      tmp = Rain;
      tmp.setLightStat(testlight);
      return tmp;
    }
    if (Cnumber == SleetSun.customNumber(testlight)) {
      tmp = SleetSun;
      tmp.setLightStat(testlight);
      return tmp;
    }
    if (Cnumber == SnowSun.customNumber(testlight)) {
      tmp = SnowSun;
      tmp.setLightStat(testlight);
      return tmp;
    }
    if (Cnumber == Sleet.customNumber(testlight)) {
      tmp = Sleet;
      tmp.setLightStat(testlight);
      return tmp;
    }
    if (Cnumber == Snow.customNumber(testlight)) {
      tmp = Snow;
      tmp.setLightStat(testlight);
      return tmp;
    }
    if (Cnumber == LightRainThunderSun.customNumber(testlight)) {
      tmp = LightRainThunderSun;
      tmp.setLightStat(testlight);
      return tmp;
    }
    if (Cnumber == RainThunder.customNumber(testlight)) {
      tmp = RainThunder;
      tmp.setLightStat(testlight);
      return tmp;
    }
    if (Cnumber == SnowThunder.customNumber(testlight)) {
      tmp = SnowThunder;
      tmp.setLightStat(testlight);
      return tmp;
    }

    if (Cnumber == SleetSunThunder.customNumber(testlight)) {
       tmp = SleetSunThunder;
       tmp.setLightStat(testlight);
       return tmp;
    }

    if (Cnumber == SnowSunThunder.customNumber(testlight)) {
       tmp = SnowSunThunder;
       tmp.setLightStat(testlight);
       return tmp;
    }

    if (Cnumber == LightRainThunder.customNumber(testlight)) {
       tmp = LightRainThunder;
       tmp.setLightStat(testlight);
       return tmp;
    }

    if (Cnumber == SleetThunder.customNumber(testlight)) {
       tmp = SleetThunder;
       tmp.setLightStat(testlight);
       return tmp;
    }
  }

  myerror.AddErr("symbol doesn't exist");
  return myerror;

}
;

bool symbolMaker::signChange(int custom1, int custom2)
{

  miSymbol symbol1, symbol2;

  symbol1 = getSymbol(custom1);
  symbol2 = getSymbol(custom2);

  if (symbol1 == symbol2)
    return false;

  if (!symbol1.change() && !symbol2.change())
    return false;

  return true;
}
;

bool symbolMaker::isDry(int custom)
{

  miSymbol tmpSym;

  tmpSym = getSymbol(custom);

  if (tmpSym == Sun || tmpSym == LightCloud || tmpSym == PartlyCloud || tmpSym
      == Cloud)
    return true;

  return false;
}
;

bool symbolMaker::isPrecip(int custom)
{

  miSymbol tmpSym;

  tmpSym = getSymbol(custom);

  if (tmpSym == Rain || tmpSym == Sleet || tmpSym == LightRain ||
      tmpSym == Snow || tmpSym == RainThunder || tmpSym == SnowThunder ||
      tmpSym == SleetSunThunder || tmpSym == SnowSunThunder ||
      tmpSym == LightRainThunder || tmpSym == SleetThunder )
    return true;

  return false;
}
;

bool symbolMaker::isShower(int custom)
{

  miSymbol tmpSym;

  tmpSym = getSymbol(custom);

  if (tmpSym == LightRainSun || tmpSym == SnowSun || tmpSym == SleetSun
      || tmpSym == LightRainThunderSun)
    return true;

  return false;
}
;

void symbolMaker::make_pos_symbols(map<int, int>& index,
    map<int, miString>& image)
{

  index.clear();
  image.clear();

  index[ErrorSymbol.customNumber(true)] = 0;
  index[Sun.customNumber(true)] = 1;
  index[LightCloud.customNumber(true)] = 2;
  index[PartlyCloud.customNumber(true)] = 3;
  index[Cloud.customNumber(true)] = 4;
  index[LightRainSun.customNumber(true)] = 5;
  index[LightRainThunderSun.customNumber(true)] = 6;
  index[SleetSun.customNumber(true)] = 7;
  index[SnowSun.customNumber(true)] = 8;
  index[LightRain.customNumber(true)] = 9;
  index[Rain.customNumber(true)] = 10;
  index[RainThunder.customNumber(true)] = 11;
  index[Sleet.customNumber(true)] = 12;
  index[Snow.customNumber(true)] = 13;
  index[SnowThunder.customNumber(true)] = 14;
  index[Fog.customNumber(true)] = 15;
  index[SleetSunThunder.customNumber(true)] = 20;
  index[SnowSunThunder.customNumber(true)] = 21;
  index[LightRainThunder.customNumber(true)] = 22;
  index[SleetThunder.customNumber(true)] = 23;

  index[Sun.customNumber(false)] = 16;
  index[PartlyCloud.customNumber(false)] = 17;
  index[LightRainSun.customNumber(false)] = 18;
  index[SnowSun.customNumber(false)] = 19;

  image[0] = ErrorSymbol.picture(true);
  image[1] = Sun.picture(true);
  image[2] = LightCloud.picture(true);
  image[3] = PartlyCloud.picture(true);
  image[4] = Cloud.picture(true);
  image[5] = LightRainSun.picture(true);
  image[6] = LightRainThunderSun.picture(true);
  image[7] = SleetSun.picture(true);
  image[8] = SnowSun.picture(true);
  image[9] = LightRain.picture(true);
  image[10] = Rain.picture(true);
  image[11] = RainThunder.picture(true);
  image[12] = Sleet.picture(true);
  image[13] = Snow.picture(true);
  image[14] = SnowThunder.picture(true);
  image[15] = Fog.picture(true);

  image[16] = Sun.picture(false);
  image[17] = PartlyCloud.picture(false);
  image[18] = LightRainSun.picture(false);
  image[19] = SnowSun.picture(false);
  image[20] = SleetSunThunder.picture(true);
  image[21] = SnowSunThunder.picture(true);
  image[22] = LightRainThunder.picture(true);
  image[23] = SleetThunder.picture(true);
}
;

vector<miSymbol> symbolMaker::compute_new(vector<paramet> AllModelData, vector<
    miTime> termin, int min = 3, int max = 6, bool compute_minmax)
{
  vector<miSymbol> tmpSymbols;
  myerror = ErrorSymbol;
  initializeModel(AllModelData);
  periods(termin, min, max, compute_minmax);
  for (vector<miTime>::size_type yet = 0; yet < termin.size(); yet++) {
    cloudMaker(termin[yet]);
    if (symbol != myerror)
      rainMaker(termin[yet]);
    if (symbol != myerror)
      stateMaker(termin[yet]);
    if (symbol != myerror)
      lightningMaker(termin[yet]);
    if (symbol != myerror)
      fogMaker(termin[yet]);

    symbol.setTime(termin[yet]);
    symbol.setLightStat(termin[yet], latitude);
    tmpSymbols.push_back(symbol);
  }
  return (tmpSymbols);
}
;

bool symbolMaker::stateMaker(miTime now)
{
  return stateMaker(symbol, FLT_MAX, agr.value(now) );
}
;

int symbolMaker::stateOfAggregateFromTemperature(float temperature)
{
  if (temperature == FLT_MAX || temperature <= DUMMY)
    return INT_MAX;

  if (temperature <= 0.5)
    return 0; //Snow

  if (temperature > 0.5 && temperature < 1.5)
    return 1; //Sleet

  return 2; //rain
}

bool symbolMaker::stateMaker(miSymbol &symbol_, float temperature,
    float stateOfAggregate )
{

  int state;
  miSymbol oldSymbol = symbol_; //Save time and lightstate

  miSymbol myErrorSymbol(ErrorSymbol);
  myErrorSymbol.AddErr("DATA ERROR > NO STATE OR TEMPERATURE DATA");

  if (stateOfAggregate == FLT_MAX || stateOfAggregate <= DUMMY)
    state = stateOfAggregateFromTemperature(temperature);
  else
    state = int(stateOfAggregate);

  // state: 0=snow, 1=sleet, 2=rain


  if (state < 0 || state > 2) {
    if (symbol_ == LightRainSun || symbol_ == LightRain || symbol_ == Rain) {
      symbol_ = myErrorSymbol;
      return false;
    }
  }

  if( symbol_ == LightRainSun ) {
    if (state == 0)
      symbol_ = SnowSun;
    if (state == 1)
      symbol_ = SleetSun;
  } else if( symbol_ == LightRain || symbol_ == Rain ) {
    if (state == 0)
      symbol_ = Snow;
    if (state == 1)
      symbol_ = Sleet;
  } else if( symbol_ == Snow ) {
    if (state == 1)
      symbol_ = Sleet;
    if (state == 2)
      symbol_ = Rain;
  } else if( symbol_ == SnowSun ) {
    if (state == 1)
      symbol_ = SleetSun;
    if (state == 2)
      symbol_ = LightRainSun;
  } else if( symbol_ == Sleet ) {
    if (state == 0)
      symbol_ = Snow;
    if (state == 2)
      symbol_ = Rain;
  } else if( symbol_ == SleetSun ) {
    if (state == 0)
      symbol_ = SnowSun;
    if (state == 2)
      symbol_ = LightRainSun;
  } else if( symbol_ == LightRainThunderSun) {
     if( state == 0 )
        symbol_ = SnowSunThunder;
     else if( state == 1 )
        symbol_ = SleetSunThunder;
  } else if( symbol_ == RainThunder ) {
     if( state == 0 )
        symbol_ = SnowThunder;
     else if( state == 1 )
        symbol_ = SleetThunder;
  } else if( symbol_ == LightRainThunder ) {
     if( state == 0 )
        symbol_ = SnowThunder;
     else if( state == 1 )
        symbol_ = SleetThunder;
  } else if( symbol_ == SleetThunder ) {
     if( state == 0 )
        symbol_ = SnowThunder;
     else if( state == 2 )
        symbol_ = RainThunder;
  } else if( symbol_ == SleetSunThunder ) {
     if( state == 0 )
        symbol_ = SnowSunThunder;
     else if( state == 2 )
        symbol_ = LightRainThunderSun;
  } else if( symbol_ == SnowThunder ) {
     if( state == 1 )
        symbol_ = SleetThunder;
     else if( state == 2 )
        symbol_ = RainThunder;
  } else if( symbol_ == SnowSunThunder ) {
     if( state == 1 )
        symbol_ = SleetSunThunder;
     else if( state == 2 )
        symbol_ = LightRainThunderSun;
  }

  //Restore time and lightstate
  symbol_.setTime( oldSymbol.getTime() );
  symbol_.setLightStat( oldSymbol.getLightStat() );

  return true;
}
;

bool symbolMaker::lightningMaker(miTime now)
{

  lightningMaker(symbol, lli.value(now));
  return true;
}

void symbolMaker::lightningMaker(miSymbol &symbol_, float lightningIndex)
{
  if (lightningIndex == FLT_MAX || lightningIndex <= DUMMY)
    return;

  int lightning = int(lightningIndex);

  if (lightning != 1)
    return;

  if( symbol_ == LightRainSun || symbol_ == LightRain || symbol_ == SleetSun )
    symbol_ = LightRainThunderSun;
  else if( symbol_ == Rain )
    symbol_ = RainThunder;
  else if( symbol_ == Snow || symbol_ == SnowSun || symbol_ == Sleet )
    symbol_ = SnowThunder;
}


void
symbolMaker::
lightningMakerNew( miSymbol &symbol_,
                   float lightningIndex )
{
   if (lightningIndex == FLT_MAX || lightningIndex <= DUMMY)
     return;

   int lightning = int(lightningIndex);

   if (lightning != 1)
     return;

   miSymbol oldSymbol = symbol_; //Save time and lightstate

   if ( symbol_ == LightRainSun )
     symbol_ = LightRainThunderSun;
   else if( symbol_ == LightRain )
      symbol_ = LightRainThunder;
   else if( symbol_ == SleetSun )
      symbol_ = SleetSunThunder;
   else if(symbol_ == Rain )
     symbol_ = RainThunder;
   else if( symbol_ == Snow )
     symbol_ = SnowThunder;
   else if( symbol_ == SnowSun )
      symbol_ = SnowSunThunder;
   else if( symbol_ == Sleet )
      symbol_ = SleetThunder;

   //Restore time and lightstate
   symbol_.setTime( oldSymbol.getTime() );
   symbol_.setLightStat( oldSymbol.getLightStat() );
}

bool
symbolMaker::
hasThunder( const miSymbol &symbol_ )
{

   if ( symbol_ == LightRainThunderSun ||
        symbol_ == LightRainThunder ||
        symbol_ == SleetSunThunder ||
        symbol_ == RainThunder ||
        symbol_ == SnowThunder ||
        symbol_ == SnowSunThunder ||
        symbol_ == SleetThunder )
      return true;
   else
      return false;
}

void
symbolMaker::
turnOffThunder( miSymbol &symbol_ )
{
   miSymbol oldSymbol = symbol_; //Save time and lightstate

   if ( symbol_ == LightRainThunderSun )
     symbol_ = LightRainSun;
   else if( symbol_ == LightRainThunder )
      symbol_ = LightRain;
   else if( symbol_ ==  SleetSunThunder)
      symbol_ = SleetSun;
   else if(symbol_ == RainThunder )
     symbol_ = Rain;
   else if( symbol_ == SnowThunder )
     symbol_ = Snow;
   else if( symbol_ == SnowSunThunder )
      symbol_ = SnowSun;
   else if( symbol_ == SleetThunder )
      symbol_ = Sleet;

   //Restore time and lightstate
   symbol_.setTime( oldSymbol.getTime() );
   symbol_.setLightStat( oldSymbol.getLightStat() );
}

void
symbolMaker::
turnOnThunder( miSymbol &symbol_ )
{
   miSymbol oldSymbol = symbol_; //Save time and lightstate

   if( symbol_ == Sun ||
       symbol_ == LightRainSun ||
       symbol_ == PartlyCloud )
      symbol_ = LightRainThunderSun;
   else if( symbol_ == Fog ||
            symbol_ == LightCloud ||
            symbol_ == Cloud ||
            symbol_ == LightRain  )
      symbol_ = LightRainThunder;
   else if( symbol_ == Rain)
      symbol_ = RainThunder;
   else if( symbol_ == SleetSun )
      symbol_ = SleetSunThunder;
   else if( symbol_ == SnowSun )
      symbol_ = SnowSunThunder;
   else if( symbol_ == Sleet )
      symbol_ = SleetThunder;
   else if( symbol_ == Snow )
      symbol_ = SnowThunder;
   else
      return;

   //Restore time and lightstate
   symbol_.setTime( oldSymbol.getTime() );
   symbol_.setLightStat( oldSymbol.getLightStat() );
}


void symbolMaker::fogMaker(miSymbol &symbol_, float fogIndex)
{
  if (fogIndex == FLT_MAX || fogIndex <= DUMMY)
    return;

  int foi = int(fogIndex);

  if (foi == 0)
    return;

  symbol_ = Fog;
}
;

bool symbolMaker::fogMaker(miTime now)
{
  fogMaker(symbol, fogi.value(now));

  return true;
}
;

vector<float> symbolMaker::water_state(vector<float> temp)
{

  int sz = temp.size();
  vector<float> state;
  float t, s;

  for (int i = 0; i < sz; i++) {
    t = temp[i];

    if (t <= 0.5) {
      s = 0;
    } else if (t >= 1.5) {
      s = 2;
    } else {
      s = 1;
    }
    state.push_back(s);

  }
  return state;

}

