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



#include <fstream>
#include "symbolMaker.h"

using namespace std;
using namespace miutil;
using namespace puMathAlgo;

// initializes the symbols with an
// internal number and the WMO-code
const float DUMMY = -999;

int      symbolMaker::MAXcustom=-1000;
int      symbolMaker::MINcustom=1000;
miSymbol symbolMaker::sun(1,false) ;
miSymbol symbolMaker::lightCloud(2,false);
miSymbol symbolMaker::partlyCloud(3,false);
miSymbol symbolMaker::cloud(4,false);
miSymbol symbolMaker::fog(5,true);
miSymbol symbolMaker::lightRainSun(10,true);
miSymbol symbolMaker::lightRain(20,true);
miSymbol symbolMaker::rain(30,true);
miSymbol symbolMaker::sleetSun(100,true);
miSymbol symbolMaker::snowSun(200,true);
miSymbol symbolMaker::sleet(300,true);
miSymbol symbolMaker::snow(400,true);
miSymbol symbolMaker::lightRainThunderSun(1000,true);
miSymbol symbolMaker::RainThunder(2000,true);
miSymbol symbolMaker::SnowThunder(3000,true);
miSymbol symbolMaker::errorSymbol(999,true);

static bool fogstatus = true;
static bool visiblestatus = false;
static bool picturestatus = false;
static bool longterm = false;
//float latitude = 60;



// reads symbol data from file, and builds the classes for symbols out of it

void symbolMaker::readSymbols(miString SymbolFname){
  MAXcustom = -1000;
  MINcustom =  1000;

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

  codeFile.open(SymbolFname.cStr(),ios::in);

  if(!codeFile)
    cerr<<"SymbolMaker error: File "<< SymbolFname<<" not found"<<endl;

  while(codeFile.good()){

    getline(codeFile,readMi);
    readMi.trim();
    if(readMi.contains("#") )
      continue;

    // flags

    if(readMi.contains("FOGSTATUS"))
       if(readMi.contains("false"))
	  fogstatus = false;

    if(readMi.contains("VISIBILITY"))
       if(readMi.contains("true"))
	  visiblestatus = true;

    if(readMi.contains("PICTURES"))
       if(readMi.contains("true"))
	  picturestatus = true;

   if(readMi.contains("LONGTERM"))
       if(readMi.contains("true"))
	  longterm = true;



    if(readMi =="{"){
      getline(codeFile,className);
      className.trim();
      getline(codeFile,normalName);
      words = normalName.split();
      normalName.replace(words[0],"");
      normalIndex = atoi(words[0].cStr());
      normalName.trim();
      if(picturestatus){
	getline(codeFile,normalPic);
	normalPic.trim();
      }
      getline(codeFile,darkName);
      darkName.trim();
      words = darkName.split();
      darkName.replace(words[0],"");
      darkIndex = atoi(words[0].cStr());
      darkName.trim();
        if(picturestatus){
	getline(codeFile,darkPic);
	darkPic.trim();
        }

      // here starts the input of the symboldependet
      // first guess for the vision
      if(visiblestatus)
	codeFile>>Vision;
      else
	Vision = 9;

      if( darkIndex > MAXcustom)
	MAXcustom = darkIndex; // 2000-11-17: normalIndex here!!

      if(normalIndex < MINcustom)
	MINcustom = normalIndex;


      normal.AddValues(normalIndex,normalName,Vision,normalPic);
      darkness.AddValues(darkIndex,darkName,Vision,darkPic);

      // and here it ends
      // compares and sorts the readed data into  the symbols

      if(className == "SUN")
	sun.AddCodes(normal,darkness);

      if(className == "LIGHTCLOUD")
	lightCloud.AddCodes(normal,darkness);

      if(className == "PARTLYCLOUD")
	partlyCloud.AddCodes(normal,darkness);

      if(className == "CLOUD")
	cloud.AddCodes(normal,darkness);

      if(className == "FOG")
	fog.AddCodes(normal,darkness);

      if(className == "LIGHTRAINSUN")
	lightRainSun.AddCodes(normal,darkness);

      if(className == "LIGHTRAIN")
	lightRain.AddCodes(normal,darkness);

      if(className == "RAIN")
	rain.AddCodes(normal,darkness);

      if(className == "SLEETSUN")
	sleetSun.AddCodes(normal,darkness);

      if(className == "SNOWSUN")
	snowSun.AddCodes(normal,darkness);

      if(className == "SLEET")
	sleet.AddCodes(normal,darkness);

      if(className == "SNOW")
	snow.AddCodes(normal,darkness);

      if(className == "LIGHTRAINTHUNDERSUN")
	lightRainThunderSun.AddCodes(normal,darkness);

      if(className == "RAINTHUNDER")
	RainThunder.AddCodes(normal,darkness);

      if(className == "SNOWTHUNDER")
	SnowThunder.AddCodes(normal,darkness);

      if(className == "ERROR")
	errorSymbol.AddCodes(normal,darkness);

    }
  }
  codeFile.close();
};




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

  for (int i=0; i < AllData.size(); i++){

    if(AllData[i].paraNO == t2m.paraNO)
      t2m  = AllData[i];

    if(AllData[i].paraNO == rrf.paraNO)
      rrf = AllData[i];

    if(AllData[i].paraNO == rrk.paraNO)
      rrk = AllData[i];

    if(AllData[i].paraNO == rrt.paraNO)
      rrt = AllData[i];

    if(AllData[i].paraNO == cs.paraNO)
      cs = AllData[i];

    if(AllData[i].paraNO == cl.paraNO)
      cl = AllData[i];

    if(AllData[i].paraNO == cm.paraNO)
      cm = AllData[i];

    if(AllData[i].paraNO == ch.paraNO)
      ch = AllData[i];

    if(AllData[i].paraNO == ct.paraNO)
      ct = AllData[i];

    if(AllData[i].paraNO == agr.paraNO)
      agr = AllData[i];

    if(AllData[i].paraNO == lli.paraNO)
      lli = AllData[i];

    if(AllData[i].paraNO == fogi.paraNO)
      fogi = AllData[i];

  }


  return (true);
};

// checks out for periods of rain and computes
// averaged values of the prec. and clouds of the 6 or 12 hours
// round the forecast termin. (dependet on the input)

void symbolMaker::periods(vector<miTime> termin,
			  int min = 3, int max =6,
			  bool compute_minmax){

  average<float> avRR(DUMMY);
  average<float> avCL(DUMMY);
  average<float> avCS(DUMMY);
  average<float> avCM(DUMMY);
  average<float> avCH(DUMMY);
  average<float> avCT(DUMMY);


  miTime runTerm;
  miTime stopTerm;
  float rrto,rrtot, rrconv, rrfront;
  float nrr1,nrr0;

  periodOfRain.erase();
  periodOfNoRain.erase();
  rrtotal.erase();
  csurf.erase();
  clow.erase();
  cmiddle.erase();
  cheight.erase();
  ctotal.erase();

  for(int i = 0; i < termin.size(); i++){
    nrr0 = 0;
    nrr1 = 0;
    rrto = DUMMY;
    rrtot =DUMMY;

    // compute new min/max
    if (compute_minmax){

      if (i == 0) {
	runTerm  = termin[i];
	runTerm.addHour(-min+1);
	stopTerm = termin[i];
	stopTerm.addHour(max);
      } else {
	runTerm= stopTerm;
	int diff;
	if ( i < termin.size()-1 ){ // not the last timepoint
	  // rhs cover half the interval to next timepoint
	  diff = miTime::hourDiff(termin[i+1],termin[i]) / 2;
	} else {
	  // last timepoint; rhs equal to lhs
	  diff = miTime::hourDiff(termin[i],runTerm);
	}
	stopTerm = termin[i];
	stopTerm.addHour(diff);
	runTerm.addHour(1);
      }
//       cerr << "--runTerm:" << runTerm
// 	   << " termin:" << termin[i]
// 	   << " stopTerm:" << stopTerm << endl;

    } else {

      runTerm  = termin[i];
      stopTerm = termin[i];

      runTerm.addHour(-min);
      stopTerm.addHour(max);
    }


    while(runTerm <= stopTerm){
      rrtot = rrt.value(runTerm);
      latitude = rrt.lat();

      if(rrtot==DUMMY){
	rrconv  = rrk.value(runTerm);
	rrfront = rrf.value(runTerm);

	if(rrconv != DUMMY && rrfront == DUMMY){
	  rrtot = rrconv;
	  latitude = rrk.lat();
	}
	else if(rrconv == DUMMY && rrfront != DUMMY){
	  rrtot = rrfront;
	  latitude = rrf.lat();
	}
	else if(rrconv != DUMMY && rrfront != DUMMY)
	  rrtot = rrfront + rrconv;
      }

      if(rrtot !=  DUMMY){
	if(rrto == DUMMY)
	  rrto = 0;
	rrto   += rrtot;
      }

      if(runTerm == termin[i]||runTerm == stopTerm){
	if(rrto > 0.6 )
	  nrr1++;
	else if(rrto < 0.2 && rrto > DUMMY)
	  nrr0++;
	avRR.add(rrto) ;
	rrto = DUMMY;
      }

      avCL.add( cl.value(runTerm ) ) ;
      avCS.add( cs.value(runTerm ) ) ;
      avCM.add( cm.value(runTerm ) ) ;
      avCH.add( ch.value(runTerm ) ) ;
      avCT.add( ct.value(runTerm ) ) ;

      runTerm.addHour(1);
    }
    periodOfRain.push(termin[i], nrr1);
    periodOfNoRain.push(termin[i],nrr0);
    rrtotal.push( termin[i], avRR.middle());
    csurf.push(   termin[i], avCS.middle());
    clow.push(    termin[i], avCL.middle());
    cmiddle.push( termin[i], avCM.middle());
    cheight.push( termin[i], avCH.middle());
    ctotal.push(  termin[i], avCT.middle());
  }
};

// checks out for clouds > 1st step

bool symbolMaker::cloudMaker(miTime now){
   myerror.AddErr("DATA ERROR > NO CLOUD DATA");
  symbol = myerror;

  if(ctotal.value(now) != DUMMY)
    symbol = sun;

  if(clow.value(now)    != DUMMY &&
     cmiddle.value(now) != DUMMY &&
     cheight.value(now) != DUMMY &&
     csurf.value(now)   != DUMMY  )
    symbol = sun;

  if (symbol == myerror)
    return(false);

  if(ctotal.value(now) >= 13 )
    symbol = lightCloud;

  if(ctotal.value(now) >  38 )
    symbol = partlyCloud;

  if(ctotal.value(now) >  86 )
    symbol = cloud  ;

  if (csurf.value(now)   > 15 ||
      clow.value(now)    > 15 ||
      cmiddle.value(now) > 15 ||
      cheight.value(now) > 25 )
    symbol = lightCloud;

  if (csurf.value(now)   > 45 ||
      clow.value(now)    > 45 ||
      cmiddle.value(now) > 45 ||
      cheight.value(now) > 55 )
    symbol = partlyCloud;

  if (csurf.value(now)   > 86 ||
      clow.value(now)    > 86 ||
      cmiddle.value(now) > 86 ||
      cheight.value(now) > 91 )
    symbol = cloud;


 //  if(fogstatus && csurf.value(now) > 75 )
//     symbol = fog;

  return(true);
};

// checks out for precipitation >  2nd step

bool symbolMaker::rainMaker(miTime now){
   myerror.AddErr("DATA ERROR > NO PRECIPITATION DATA");

  float rrNow   = rrtotal.value(now);
  float pRain   = periodOfRain.value(now);
  float noRain  = periodOfNoRain.value(now);

  if(rrtotal.value(now) == DUMMY){
    symbol = myerror;
    return(false);
  }

  if(longterm)
    rrNow/=2;



  // changes: 2002-08-21
  // range 0.1 -> 0.2
  // range 0.3 -> 0.6



  if(rrNow> 0.2 || pRain > 0 ){

    if(symbol == sun   ||  symbol == lightCloud )
      symbol = lightRainSun ;

    if(symbol == partlyCloud) {
      if(  rrNow<= 0.6  || noRain >= pRain)
	symbol = lightRainSun;
      else
	symbol = lightRain;
    }

    if(symbol == cloud &&  rrNow > 0.2 )
      symbol = lightRain;

    if(symbol == lightRain && rrNow> 5 )
      symbol = rain;
  }


  return(true);
};

// checks out the weather relevant temperatures (checks for snow etc.)
// only relevant if precipitation occurs > 3rd step

bool symbolMaker::tempMaker(miTime now){

  float temperature;

  myerror.AddErr("DATA ERROR > NO TEMPERATURE DATA");

  temperature = t2m.value(now);

  if (temperature == DUMMY){
    if (symbol == lightRainSun ||
	symbol == lightRain    ||
	symbol == rain        ){
      symbol = myerror;
      return(false);
    }
  }

  if ( symbol == lightRainSun ){
    if(temperature <= 0.5 )
      symbol = snowSun;
    if(temperature > 0.5 && temperature < 1.5 )
      symbol = sleetSun;
  }

  if( symbol == lightRain || symbol == rain){
    if(temperature <= 0.5 )
      symbol = snow;
    if(temperature > 0.5 && temperature < 1.5 )
      symbol = sleet;
  }
  return(true);
};

// main program


vector<miSymbol>
symbolMaker::
computeWithoutStateOfAggregate( const vector<paramet> &AllModelData,
		                          const vector<miTime> &termin,
				      				  int min,int max)
{
  vector<miSymbol> tmpSymbols;
  myerror=errorSymbol;
  initializeModel(AllModelData);
  periods(termin,min,max);

  for( int yet = 0 ; yet < termin.size(); yet++){
    cloudMaker( termin[yet] );
    if(symbol != myerror)
      rainMaker(  termin[yet] );

    symbol.setTime(termin[yet]);
    symbol.setLightStat(termin[yet],latitude);
    tmpSymbols.push_back(symbol);
  }

  return(tmpSymbols);

};



vector<miSymbol> symbolMaker::compute(vector<paramet> AllModelData,vector<miTime> termin,
				      int min,int max){
  vector<miSymbol> tmpSymbols;
   myerror=errorSymbol;
  initializeModel(AllModelData);
  periods(termin,min,max);
  for( int yet = 0 ; yet < termin.size(); yet++){
    cloudMaker( termin[yet] );
    if(symbol != myerror)
      rainMaker(  termin[yet] );
    if(symbol != myerror)
      tempMaker(  termin[yet] );

    symbol.setTime(termin[yet]);
    symbol.setLightStat(termin[yet],latitude);
    tmpSymbols.push_back(symbol);
  }
  return(tmpSymbols);

};


miSymbol symbolMaker::getSymbol(int Cnumber){

  miSymbol tmp;

  for ( int testlight = 1; testlight >= 0; testlight-- ){

   if(Cnumber == sun.customNumber(testlight)){
     tmp = sun;
     tmp.setLightStat(testlight);
     return tmp;
   }
   if(Cnumber == lightCloud.customNumber(testlight)){
     tmp =  lightCloud;
     tmp.setLightStat(testlight);
     return tmp;
   }
   if(Cnumber ==  partlyCloud.customNumber(testlight)){
     tmp = partlyCloud;
     tmp.setLightStat(testlight);
     return tmp;
   }
   if(Cnumber ==  cloud.customNumber(testlight)){
     tmp = cloud;
     tmp.setLightStat(testlight);
     return tmp;
   }
   if(Cnumber ==  fog.customNumber(testlight)){
     tmp = fog ;
     tmp.setLightStat(testlight);
     return tmp;
   }
   if(Cnumber ==  lightRainSun.customNumber(testlight)){
       tmp = lightRainSun;
       tmp.setLightStat(testlight);
       return tmp;
   }
   if(Cnumber ==  lightRain.customNumber(testlight)){
     tmp = lightRain;
     tmp.setLightStat(testlight);
     return tmp;
   }
   if(Cnumber ==  rain.customNumber(testlight)){
     tmp = rain;
     tmp.setLightStat(testlight);
     return tmp;
   }
   if(Cnumber ==  sleetSun.customNumber(testlight)){
     tmp =  sleetSun;
     tmp.setLightStat(testlight);
     return tmp;
   }
   if(Cnumber ==  snowSun.customNumber(testlight)){
     tmp =  snowSun;
     tmp.setLightStat(testlight);
     return tmp;
   }
   if(Cnumber ==  sleet.customNumber(testlight)){
     tmp = sleet;
     tmp.setLightStat(testlight);
     return tmp;
   }
   if(Cnumber ==  snow.customNumber(testlight)){
     tmp = snow;
     tmp.setLightStat(testlight);
     return tmp;
   }
   if(Cnumber ==  lightRainThunderSun.customNumber(testlight)){
     tmp = lightRainThunderSun;
     tmp.setLightStat(testlight);
     return tmp;
   }
   if(Cnumber ==  RainThunder.customNumber(testlight)){
     tmp =  RainThunder;
     tmp.setLightStat(testlight);
     return tmp;
   }
   if(Cnumber ==  SnowThunder.customNumber(testlight)){
     tmp = SnowThunder;
     tmp.setLightStat(testlight);
     return tmp;
   }
 }

  myerror.AddErr("symbol doesn't exist");
  return myerror;

};



bool symbolMaker::signChange(int custom1, int custom2){

  miSymbol symbol1, symbol2;

  symbol1 = getSymbol(custom1);
  symbol2 = getSymbol(custom2);

  if(symbol1 == symbol2)
    return false;

  if(!symbol1.change() && !symbol2.change())
    return false;

  return true;
};


bool symbolMaker::isDry(int custom ){

  miSymbol tmpSym;

  tmpSym = getSymbol( custom);

  if( tmpSym == sun         ||
      tmpSym == lightCloud  ||
      tmpSym == partlyCloud ||
      tmpSym == cloud )
    return true;

  return false;
};



bool symbolMaker::isPrecip(int custom ){

  miSymbol tmpSym;

  tmpSym = getSymbol( custom);

  if( tmpSym == rain         ||
      tmpSym == sleet        ||
      tmpSym == lightRain    ||
      tmpSym == snow         ||
      tmpSym == RainThunder  ||
      tmpSym == SnowThunder  )
    return true;

  return false;
};


bool symbolMaker::isShower(int custom ){

  miSymbol tmpSym;

  tmpSym = getSymbol( custom);

  if( tmpSym == lightRainSun   ||
      tmpSym == snowSun        ||
      tmpSym == sleetSun       ||
      tmpSym == lightRainThunderSun )
    return true;

  return false;
};

void symbolMaker::make_pos_symbols(map<int,int>& index,
				   map<int,miString>& image){

  index.clear();
  image.clear();

  index[errorSymbol.customNumber(true)] = 0;
  index[sun.customNumber(true)] = 1 ;
  index[lightCloud.customNumber(true)] = 2;
  index[partlyCloud.customNumber(true)] = 3;
  index[cloud.customNumber(true)] = 4;
  index[lightRainSun.customNumber(true)] = 5;
  index[lightRainThunderSun.customNumber(true)] = 6;
  index[sleetSun.customNumber(true)] = 7;
  index[snowSun.customNumber(true)] = 8;
  index[lightRain.customNumber(true)] = 9;
  index[rain.customNumber(true)] = 10;
  index[RainThunder.customNumber(true)] = 11;
  index[sleet.customNumber(true)] = 12;
  index[snow.customNumber(true)] = 13;
  index[SnowThunder.customNumber(true)] = 14;
  index[fog.customNumber(true)] = 15;

  index[sun.customNumber(false)] = 16;
  index[partlyCloud.customNumber(false)] = 17;
  index[lightRainSun.customNumber(false)] =18;
  index[snowSun.customNumber(false)] = 19;


  image[0]  = errorSymbol.picture(true);
  image[1]  = sun.picture(true);
  image[2]  = lightCloud.picture(true);
  image[3]  = partlyCloud.picture(true);
  image[4]  = cloud.picture(true);
  image[5]  = lightRainSun.picture(true);
  image[6]  = lightRainThunderSun.picture(true);
  image[7]  = sleetSun.picture(true);
  image[8]  = snowSun.picture(true);
  image[9]  = lightRain.picture(true);
  image[10] = rain.picture(true);
  image[11] = RainThunder.picture(true);
  image[12] = sleet.picture(true);
  image[13] = snow.picture(true);
  image[14] = SnowThunder.picture(true);
  image[15] = fog.picture(true);

  image[16] = sun.picture(false);
  image[17] = partlyCloud.picture(false);
  image[18] = lightRainSun.picture(false);
  image[19] = snowSun.picture(false);


};




vector<miSymbol> symbolMaker::compute_new(vector<paramet> AllModelData,vector<miTime> termin,
					  int min = 3,int max = 6,
					  bool compute_minmax)
{
  vector<miSymbol> tmpSymbols;
   myerror=errorSymbol;
  initializeModel(AllModelData);
  periods(termin,min,max,compute_minmax);
  for( int yet = 0 ; yet < termin.size(); yet++){
    cloudMaker( termin[yet] );
    if(symbol != myerror)
      rainMaker(  termin[yet] );
    if(symbol != myerror)
      stateMaker(  termin[yet] );
    if(symbol != myerror)
      lightningMaker(termin[yet]);
    if(symbol != myerror)
      fogMaker(termin[yet]);

    symbol.setTime(termin[yet]);
    symbol.setLightStat(termin[yet],latitude);
    tmpSymbols.push_back(symbol);
  }
  return(tmpSymbols);
};




bool
symbolMaker::
stateMaker(miTime now)
{
	return stateMaker( symbol, FLT_MAX, agr.value( now ), FLT_MAX, FLT_MAX );
};

int
symbolMaker::
stateOfAggregateFromTemperature( float temperature )
{
	if( temperature == FLT_MAX || temperature <= DUMMY )
		return INT_MAX;

	if(temperature <= 0.5 )
	   return 0; //Snow

	if(temperature > 0.5 && temperature < 1.5 )
		return 1; //Sleet

	return 2; //rain
}


bool
symbolMaker::
stateMaker( miSymbol &symbol_,
		      float temperature,
			   float stateOfAggregate,
		      float lightningIndex,
		      float fogIndex )
{

  int state;
  miSymbol oldSymbol = symbol_; //Save time and lightstate

  miSymbol myErrorSymbol( errorSymbol );
  myErrorSymbol.AddErr("DATA ERROR > NO STATE OR TEMPERATURE DATA");

  if( stateOfAggregate == FLT_MAX || stateOfAggregate <= DUMMY)
	  state = stateOfAggregateFromTemperature( temperature );
  else
	  state = int( stateOfAggregate );


  // state: 0=snow, 1=sleet, 2=rain


  if (state < 0 || state > 2){
    if (symbol_ == lightRainSun ||
   	  symbol_ == lightRain    ||
	     symbol_ == rain )
    {
      symbol_ = myErrorSymbol;
      return false;
    }
  }

  if ( symbol_ == lightRainSun ){
    if(state == 0 )
      symbol_ = snowSun;
    if(state == 1 )
      symbol_ = sleetSun;
  }

  if( symbol_ == lightRain || symbol_ == rain){
    if(state == 0 )
      symbol_ = snow;
    if(state == 1 )
      symbol_ = sleet;
  }

  if( symbol_ == snow ){
    if(state == 1 )
      symbol_ = sleet;
    if(state == 2 )
      symbol_ = rain;
  }

  if( symbol_ == snowSun){
    if(state == 1 )
      symbol_ = sleetSun;
    if(state == 2 )
      symbol_ = lightRainSun;
  }


  if( symbol_ == sleet) {
    if(state == 0 )
      symbol_ = snow;
    if(state == 2 )
      symbol_ = rain;
  }

  if( symbol_ == sleetSun) {
    if(state == 0 )
      symbol_ = snowSun;
    if(state == 2 )
      symbol_ = lightRainSun;
  }

  lightningMaker( symbol_, lightningIndex );
  fogMaker( symbol_, fogIndex );

  //Restore time and lightstate
  symbol_.setTime( oldSymbol.getTime() );
  symbol_.setLightStat( oldSymbol.getLightStat() );

  return true;
};




bool
symbolMaker::lightningMaker(miTime now){

	lightningMaker( symbol, lli.value( now ) );
	return true;
};


void
symbolMaker::
lightningMaker( miSymbol &symbol_, float lightningIndex )
{
	if( lightningIndex == FLT_MAX || lightningIndex <= DUMMY )
		return;

	int lightning = int( lightningIndex );

	if( lightning != 1 )
		return;

	if( symbol_ == lightRainSun ||
		 symbol_ == lightRain    ||
       symbol_ == sleetSun       )
		symbol_ = lightRainThunderSun;

	if( symbol_ == rain )
	  symbol_ = RainThunder;

	if(symbol_ == snow    ||
		symbol_ == snowSun ||
      symbol_ == sleet     )
		symbol_ = SnowThunder;
};


void
symbolMaker::
fogMaker( miSymbol &symbol_, float fogIndex)
{
	if( fogIndex == FLT_MAX || fogIndex <= DUMMY )
		return;

  int foi = int( fogIndex );

  if (foi == 0 )
    return;

  symbol_ = fog;
};



bool
symbolMaker::
fogMaker( miTime now )
{
	fogMaker( symbol, fogi.value(now) );

	return true;
};


vector<float> symbolMaker::water_state(vector<float> temp)
{

  int sz = temp.size();
  vector<float> state;
  float t,s;

  for( int i = 0; i < sz ; i++) {
    t = temp[i];

    if ( t <= 0.5 ) {
      s = 0;
    }
    else if ( t >= 1.5 ) {
      s = 2;
    }
    else {
      s = 1;
    }
    state.push_back(s);

  }
  return state;

}











