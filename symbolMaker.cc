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

int symbolMaker::maxCustom() { return MAXcustom;}
int symbolMaker::minCustom() { return MINcustom;}

vector<miSymbol> symbolbuffer;

bool fogstatus = true;
bool thunderstatus = false;
bool visiblestatus = false;
bool picturestatus = false;
bool longterm = false;
bool useDynamicRainLimits=false;


// initialise symbol generation at runtime to avoid linker trouble!
void symbolMaker::initializeSymbols()
{
  vector<miSymbol> sbuffer(20);

  sbuffer[Sun]          = miSymbol(1, false);
  sbuffer[LightCloud]   = miSymbol(2, false);
  sbuffer[PartlyCloud]  = miSymbol(3, false);
  sbuffer[Cloud]        = miSymbol(4, false);
  sbuffer[Fog]          = miSymbol(5, true);
  sbuffer[LightRainSun] = miSymbol(10, true);
  sbuffer[LightRain]    = miSymbol(20, true);
  sbuffer[Rain]         = miSymbol(30, true);
  sbuffer[SleetSun]     = miSymbol(100, true);
  sbuffer[SnowSun]      = miSymbol(200, true);
  sbuffer[Sleet]        = miSymbol(300, true);
  sbuffer[Snow]         = miSymbol(400, true);
  sbuffer[LightRainThunderSun] = miSymbol(1000, true);
  sbuffer[RainThunder]      = miSymbol(2000, true);
  sbuffer[SnowThunder]      = miSymbol(3000, true);
  sbuffer[SleetSunThunder]  = miSymbol(4000, true);
  sbuffer[SnowSunThunder]   = miSymbol(5000, true);
  sbuffer[LightRainThunder] = miSymbol( 6000, true );
  sbuffer[SleetThunder]     = miSymbol(7000, true);
  sbuffer[ErrorSymbol]      = miSymbol(999, true);

  symbolbuffer=sbuffer;
}

miSymbol symbolMaker::type2symbol(enum symbolMaker::Symboltype type)
{
  if ( type > symbolbuffer.size() || type < 0)
   return myerror;
  return symbolbuffer[type];
}


int symbolMaker::visibility(enum symbolMaker::Symboltype type)
{
  if ( type > symbolbuffer.size() || type < 0) return 0;
  return symbolbuffer[type].vis();
}


int symbolMaker::customNumber(Symboltype type, bool lights)
{
  return type2symbol(type).customNumber(lights);
}

miutil::miString symbolMaker::picture(Symboltype type, bool lights)
{
  return type2symbol(type).picture(lights);
}



// reads symbol data from file, and builds the classes for symbols out of it


void symbolMaker::readSymbols(miString SymbolFname)
{
  initializeSymbols();
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
        symbolbuffer[Sun].AddCodes(normal, darkness);

      if (className == "LIGHTCLOUD")
        symbolbuffer[LightCloud].AddCodes(normal, darkness);

      if (className == "PARTLYCLOUD")
        symbolbuffer[PartlyCloud].AddCodes(normal, darkness);

      if (className == "CLOUD")
        symbolbuffer[Cloud].AddCodes(normal, darkness);

      if (className == "FOG")
        symbolbuffer[Fog].AddCodes(normal, darkness);

      if (className == "LIGHTRAINSUN")
        symbolbuffer[LightRainSun].AddCodes(normal, darkness);

      if (className == "LIGHTRAIN")
        symbolbuffer[LightRain].AddCodes(normal, darkness);

      if (className == "RAIN")
        symbolbuffer[Rain].AddCodes(normal, darkness);

      if (className == "SLEETSUN")
        symbolbuffer[SleetSun].AddCodes(normal, darkness);

      if (className == "SNOWSUN")
        symbolbuffer[SnowSun].AddCodes(normal, darkness);

      if (className == "SLEET")
        symbolbuffer[Sleet].AddCodes(normal, darkness);

      if (className == "SNOW")
        symbolbuffer[Snow].AddCodes(normal, darkness);

      if (className == "LIGHTRAINTHUNDERSUN")
        symbolbuffer[LightRainThunderSun].AddCodes(normal, darkness);

      if (className == "RAINTHUNDER")
        symbolbuffer[RainThunder].AddCodes(normal, darkness);

      if (className == "SNOWTHUNDER")
        symbolbuffer[SnowThunder].AddCodes(normal, darkness);

      if (className == "SLEETSUNTHUNDER")
         symbolbuffer[SleetSunThunder].AddCodes(normal, darkness);

      if (className == "SNOWSUNTHUNDER")
         symbolbuffer[SnowSunThunder].AddCodes(normal, darkness);

      if (className == "LIGHTRAINTHUNDER")
         symbolbuffer[LightRainThunder].AddCodes(normal, darkness);

      if (className == "SLEETTHUNDER")
         symbolbuffer[SleetThunder].AddCodes(normal, darkness);

      if (className == "ERROR")
        symbolbuffer[ErrorSymbol].AddCodes(normal, darkness);

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
  symbol = ErrorSymbol;

  if (ctotal.value(now) != DUMMY)
    symbol = Sun;

  if (clow.value(now) != DUMMY && cmiddle.value(now) != DUMMY && cheight.value(
      now) != DUMMY && csurf.value(now) != DUMMY)
    symbol = Sun;

  if (symbol == ErrorSymbol)
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
    symbol = ErrorSymbol;
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
      symbol = ErrorSymbol;
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
  vector<miSymbol> resultSymbols;

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

  initializeModel(AllModelData);
  periods(termin, min, max);

  for (vector<miTime>::size_type yet = 0; yet < termin.size(); yet++) {
    cloudMaker(termin[yet]);
    if (symbol != ErrorSymbol)
      rainMaker(termin[yet]);

    if( thunderstatus && symbol != ErrorSymbol ) {
       if( useNewLightningMaker )
          lightningMakerNew( lli.value( termin[yet] ) );
       else
          lightningMaker(  lli.value( termin[yet] ) );
    }

    if( fogstatus && symbol != ErrorSymbol )
       fogMaker( fogi.value( termin[yet] ) );


    miSymbol result = type2symbol(symbol);

    result.setTime(termin[yet]);
    result.setLightStat(termin[yet], latitude);
    resultSymbols.push_back(result);
  }

  return resultSymbols;

}
;

vector<miSymbol> symbolMaker::compute(vector<paramet> AllModelData, vector<
    miTime> termin, int min, int max)
{
  vector<miSymbol> resultSymbols;
  miSymbol result;

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


  initializeModel(AllModelData);
  periods(termin, min, max);
  for (vector<miTime>::size_type yet = 0; yet < termin.size(); yet++) {
    cloudMaker(termin[yet]);
    if (symbol != ErrorSymbol)
      rainMaker(termin[yet]);
    if (symbol != ErrorSymbol)
      tempMaker(termin[yet]);

    result=type2symbol(symbol);
    result.setTime(termin[yet]);
    result.setLightStat(termin[yet], latitude);
    resultSymbols.push_back(result);
  }
  return (resultSymbols);

}
;

miSymbol symbolMaker::getSymbol(int Cnumber)
{

  miSymbol result;

  for (int testlight = 1; testlight >= 0; testlight--) {
    Symboltype symbolt = getSymboltype(Cnumber, testlight);
    if(symbolt != ErrorSymbol ) {
      result =  type2symbol(symbolt);
      result.setLightStat(testlight);
      return result;
    }
  }

  myerror.AddErr("symbol doesn't exist");
  return myerror;

}


symbolMaker::Symboltype symbolMaker::getSymboltype(int Cnumber)
{
  for (int testlight = 1; testlight >= 0; testlight--) {
    Symboltype symbolt = getSymboltype(Cnumber, testlight);
    if(symbolt != ErrorSymbol ) 
      return symbolt;
  }
  return ErrorSymbol; 
}



symbolMaker::Symboltype symbolMaker::getSymboltype(int Cnumber, int testlight)
{

  if (Cnumber == customNumber(Sun,testlight))
    return Sun;
  if (Cnumber ==  customNumber(LightCloud,testlight))
    return LightCloud;
  if (Cnumber ==  customNumber(PartlyCloud,testlight))
    return PartlyCloud;
  if (Cnumber ==  customNumber(Cloud,testlight))
    return Cloud;
  if (Cnumber ==  customNumber(Fog,testlight))
    return Fog;
  if (Cnumber == customNumber(LightRainSun,testlight))
    return LightRainSun;
  if (Cnumber == customNumber(LightRain,testlight))
      return LightRain;
  if (Cnumber == customNumber(Rain,testlight))
      return Rain;
  if (Cnumber == customNumber(SleetSun,testlight))
    return SleetSun;
  if (Cnumber == customNumber(SnowSun,testlight))
    return SnowSun;
  if (Cnumber == customNumber(Sleet,testlight))
    return Sleet;
  if (Cnumber ==customNumber( Snow,testlight))
    return Snow;
  if (Cnumber == customNumber(LightRainThunderSun,testlight))
    return LightRainThunderSun;
  if (Cnumber == customNumber(RainThunder,testlight))
    return RainThunder;
  if (Cnumber == customNumber(SnowThunder,testlight))
    return SnowThunder;
  if (Cnumber == customNumber(SleetSunThunder,testlight))
    return SleetSunThunder;
  if (Cnumber == customNumber(SnowSunThunder,testlight))
    return SnowSunThunder;
  if (Cnumber == customNumber(LightRainThunder,testlight))
    return LightRainThunder;
  if (Cnumber == customNumber(SleetThunder,testlight))
    return SleetThunder;

  return ErrorSymbol;
}



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


bool symbolMaker::isDry(int custom)
{
  int weatherType = getSymboltype(custom);

  if (weatherType == Sun || weatherType == LightCloud || weatherType == PartlyCloud ||
      weatherType == Cloud)
    return true;

  return false;
}


bool symbolMaker::isPrecip(int custom)
{
 int weatherType = getSymboltype(custom);
 
 
  if (weatherType == Rain || weatherType == Sleet || weatherType == LightRain ||
      weatherType == Snow || weatherType == RainThunder || weatherType == SnowThunder ||
      weatherType == SleetSunThunder || weatherType == SnowSunThunder ||
      weatherType == LightRainThunder || weatherType == SleetThunder )
    return true;

  return false;
}
;

bool symbolMaker::isShower(int custom)
{

  int weatherType = getSymboltype(custom);

  if (weatherType == LightRainSun || weatherType == SnowSun || weatherType == SleetSun
      || weatherType == LightRainThunderSun)
    return true;

  return false;
}
;

void symbolMaker::make_pos_symbols(map<int, int>& index,
    map<int, miString>& image)
{

  index.clear();
  image.clear();

  index[customNumber(ErrorSymbol,true)] = 0;
  index[customNumber(Sun,true)] = 1;
  index[customNumber(LightCloud,true)] = 2;
  index[customNumber(PartlyCloud,true)] = 3;
  index[customNumber(Cloud,true)] = 4;
  index[customNumber(LightRainSun,true)] = 5;
  index[customNumber(LightRainThunderSun,true)] = 6;
  index[customNumber(SleetSun,true)] = 7;
  index[customNumber(SnowSun,true)] = 8;
  index[customNumber(LightRain,true)] = 9;
  index[customNumber(Rain,true)] = 10;
  index[customNumber(RainThunder,true)] = 11;
  index[customNumber(Sleet,true)] = 12;
  index[customNumber(Snow,true)] = 13;
  index[customNumber(SnowThunder,true)] = 14;
  index[customNumber(Fog,true)] = 15;
  index[customNumber(SleetSunThunder,true)] = 20;
  index[customNumber(SnowSunThunder,true)] = 21;
  index[customNumber(LightRainThunder,true)] = 22;
  index[customNumber(SleetThunder,true)] = 23;

  index[customNumber(Sun,false)] = 16;
  index[customNumber(PartlyCloud,false)] = 17;
  index[customNumber(LightRainSun,false)] = 18;
  index[customNumber(SnowSun,false)] = 19;

  image[0] = picture(ErrorSymbol,true);
  image[1] = picture(Sun,true);
  image[2] = picture(LightCloud,true);
  image[3] = picture(PartlyCloud,true);
  image[4] = picture(Cloud,true);
  image[5] = picture(LightRainSun,true);
  image[6] = picture(LightRainThunderSun,true);
  image[7] = picture(SleetSun,true);
  image[8] = picture(SnowSun,true);
  image[9] = picture(LightRain,true);
  image[10] = picture(Rain,true);
  image[11] = picture(RainThunder,true);
  image[12] = picture(Sleet,true);
  image[13] = picture(Snow,true);
  image[14] = picture(SnowThunder,true);
  image[15] = picture(Fog,true);

  image[16] = picture(Sun,false);
  image[17] = picture(PartlyCloud,false);
  image[18] = picture(LightRainSun,false);
  image[19] = picture(SnowSun,false);
  image[20] = picture(SleetSunThunder,true);
  image[21] = picture(SnowSunThunder,true);
  image[22] = picture(LightRainThunder,true);
  image[23] = picture(SleetThunder,true);
}
;

vector<miSymbol> symbolMaker::compute_new(vector<paramet> AllModelData, vector<
    miTime> termin, int min = 3, int max = 6, bool compute_minmax)
{
  vector<miSymbol> resultSymbols;
  miSymbol result;

  initializeModel(AllModelData);
  periods(termin, min, max, compute_minmax);

  for (vector<miTime>::size_type yet = 0; yet < termin.size(); yet++) {
    cloudMaker(termin[yet]);
    if (symbol != ErrorSymbol)
      rainMaker(termin[yet]);
    if (symbol != ErrorSymbol)
      stateMaker(termin[yet]);
    if (symbol != ErrorSymbol)
      lightningMaker(termin[yet]);
    if (symbol != ErrorSymbol)
      fogMaker(termin[yet]);

    result = type2symbol(symbol);
    result.setTime(termin[yet]);
    result.setLightStat(termin[yet], latitude);
    resultSymbols.push_back(result);
  }
  return (resultSymbols);
}



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


bool symbolMaker::stateMaker(miTime now)
{

  float stateOfAggregate =  agr.value(now);
  float temperature      = t2m.value(now);
  int state;
 
  if (stateOfAggregate == FLT_MAX || stateOfAggregate <= DUMMY)
    state = stateOfAggregateFromTemperature(temperature);
  else
    state = int(stateOfAggregate);

  // state: 0=snow, 1=sleet, 2=rain


  if (state < 0 || state > 2) {
    if (symbol == LightRainSun || symbol == LightRain || symbol == Rain) {
      myerror.AddErr("DATA ERROR > NO STATE OR TEMPERATURE DATA");
      symbol = ErrorSymbol;
      return false;
    }
  }

  if( symbol == LightRainSun ) {
    if (state == 0)
      symbol = SnowSun;
    if (state == 1)
      symbol = SleetSun;
  } else if( symbol == LightRain || symbol == Rain ) {
    if (state == 0)
      symbol  = Snow;
    if (state == 1)
      symbol  = Sleet;
  } else if( symbol == Snow ) {
    if (state == 1)
      symbol  = Sleet;
    if (state == 2)
      symbol  = Rain;
  } else if( symbol == SnowSun ) {
    if (state == 1)
      symbol  = SleetSun;
    if (state == 2)
      symbol  = LightRainSun;
  } else if( symbol == Sleet ) {
    if (state == 0)
      symbol  = Snow;
    if (state == 2)
      symbol  = Rain;
  } else if( symbol == SleetSun ) {
    if (state == 0)
      symbol  = SnowSun;
    if (state == 2)
      symbol  = LightRainSun;
  } else if( symbol == LightRainThunderSun) {
     if( state == 0 )
        symbol  = SnowSunThunder;
     else if( state == 1 )
        symbol = SleetSunThunder;
  } else if( symbol  == RainThunder ) {
     if( state == 0 )
        symbol  = SnowThunder;
     else if( state == 1 )
        symbol = SleetThunder;
  } else if( symbol == LightRainThunder ) {
     if( state == 0 )
        symbol  = SnowThunder;
     else if( state == 1 )
        symbol  = SleetThunder;
  } else if( symbol == SleetThunder ) {
     if( state == 0 )
        symbol  = SnowThunder;
     else if( state == 2 )
        symbol  = RainThunder;
  } else if( symbol == SleetSunThunder ) {
     if( state == 0 )
        symbol  = SnowSunThunder;
     else if( state == 2 )
        symbol  = LightRainThunderSun;
  } else if( symbol == SnowThunder ) {
     if( state == 1 )
        symbol  = SleetThunder;
     else if( state == 2 )
        symbol  = RainThunder;
  } else if( symbol == SnowSunThunder ) {
     if( state == 1 )
        symbol = SleetSunThunder;
     else if( state == 2 )
        symbol = LightRainThunderSun;
  }

  return true;
}
;


void symbolMaker::lightningMaker( miTime now)
{
  float lightningIndex= lli.value(now);


  if (lightningIndex == FLT_MAX || lightningIndex <= DUMMY)
    return;

  int lightning = int(lightningIndex);

  if (lightning != 1)
    return;

  if( symbol == LightRainSun || symbol  == LightRain || symbol == SleetSun )
    symbol = LightRainThunderSun;
  else if( symbol == Rain )
    symbol = RainThunder;
  else if( symbol == Snow || symbol == SnowSun || symbol == Sleet )
    symbol = SnowThunder;
}


void symbolMaker::lightningMakerNew(  miTime now )
{
  float lightningIndex= lli.value(now);

   if (lightningIndex == FLT_MAX || lightningIndex <= DUMMY)
     return;

   int lightning = int(lightningIndex);

   if (lightning != 1)
     return;



   if ( symbol == LightRainSun )
     symbol = LightRainThunderSun;
   else if( symbol == LightRain )
      symbol = LightRainThunder;
   else if( symbol == SleetSun )
      symbol = SleetSunThunder;
   else if(symbol == Rain )
     symbol = RainThunder;
   else if( symbol == Snow )
     symbol = SnowThunder;
   else if( symbol == SnowSun )
      symbol = SnowSunThunder;
   else if( symbol == Sleet )
      symbol = SleetThunder;

}

bool symbolMaker::hasThunder( )
{

   if ( symbol == LightRainThunderSun ||
        symbol == LightRainThunder ||
        symbol == SleetSunThunder ||
        symbol == RainThunder ||
        symbol == SnowThunder ||
        symbol == SnowSunThunder ||
        symbol == SleetThunder )
      return true;
   else
      return false;
}

void symbolMaker::turnOffThunder()
{

   if ( symbol == LightRainThunderSun )
     symbol = LightRainSun;
   else if( symbol == LightRainThunder )
      symbol = LightRain;
   else if( symbol ==  SleetSunThunder)
      symbol = SleetSun;
   else if(symbol == RainThunder )
     symbol = Rain;
   else if( symbol == SnowThunder )
     symbol = Snow;
   else if( symbol == SnowSunThunder )
      symbol = SnowSun;
   else if( symbol == SleetThunder )
      symbol = Sleet;

}

void symbolMaker::turnOnThunder()
{

   if( symbol == Sun ||
       symbol == LightRainSun ||
       symbol == PartlyCloud )
      symbol = LightRainThunderSun;
   else if( symbol == Fog ||
            symbol == LightCloud ||
            symbol == Cloud ||
            symbol == LightRain  )
      symbol = LightRainThunder;
   else if( symbol == Rain)
      symbol = RainThunder;
   else if( symbol == SleetSun )
      symbol = SleetSunThunder;
   else if( symbol == SnowSun )
      symbol = SnowSunThunder;
   else if( symbol == Sleet )
      symbol = SleetThunder;
   else if( symbol == Snow )
      symbol = SnowThunder;
   else
      return;

}


void symbolMaker::fogMaker(float fogIndex)
{
  if (fogIndex == FLT_MAX || fogIndex <= DUMMY)
    return;

  int foi = int(fogIndex);

  if (foi == 0)
    return;

  symbol = Fog;
}
;

void  symbolMaker::fogMaker(miTime now)
{
  fogMaker(fogi.value(now));
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

miSymbol symbolMaker::getErrorSymbol()
{ 
  return type2symbol(ErrorSymbol);
}
