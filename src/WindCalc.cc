
#include "WindCalc.h"
#include <iostream>
#include <fstream>

bool WindCalc::readFactors(const miString& filename){
  cerr << "++ READ wind-information from:" << filename << endl;

  // first clean up
  wind_factors.clear();
  gust_factors.clear();

  ifstream f(filename.cStr());
  if (!f) {
    cerr << "ERROR opening file:" << filename << endl;
    return false;
  }
  
  miString buf;
  miString syntax = "(Stable|Neutral|Instable)_heightinmetre_(Gust|Mean) = floatvalue";
  
  while (getline(f, buf)){
    buf.trim();
    if (buf.length()==0) continue;
    if (buf[0]=='#') continue;

    vector<miString> vs= buf.split("=");

    if ( vs.size() != 2 ) {
      cerr << "Warning, illegal line in wind-information file:"
	   << buf << endl
	   << " Syntax is:" << syntax << endl;
      continue;
    }
    if ( !vs[1].isNumber() ){
      cerr << "Warning, illegal entry in wind-information file:"
	   << buf << endl
	   << " Syntax is:" << syntax << endl;
      continue;
    }
    float value = vs[1].toFloat();

    vector<miString> vvs = vs[0].split("_");
    if ( vvs.size() != 3){
      cerr << "Warning, illegal entry in wind-information file:"
	   << buf << endl
	   << " Syntax is:" << syntax << endl;
      continue;
    }
    
    int stability = 0;
    if ( vvs[0].downcase() == "stable" ){
      stability = 1;
    } else if ( vvs[0].downcase() == "instable" ){
      stability = -1;
    }

    if ( !vvs[1].isInt() ){
      cerr << "Warning, illegal entry in wind-information file:"
	   << buf << endl
	   << " Syntax is:" << syntax << endl;
      continue;
    }
    int height = vvs[1].toInt();
    
    if ( vvs[2].downcase() == "gust" ){
      gust_factors[height][stability] = value;
    } else if ( vvs[2].downcase() == "mean" ){
      wind_factors[height][stability] = value;
    } else {
      cerr << "Warning, expected GUST or MEAN in:"
	   << buf << endl
	   << " Syntax is:" << syntax << endl;
      continue;
    }
  }
}

float WindCalc::meanWind(float wind10m, int stability, int height){
  if ( wind_factors.count(height) == 0 ||
       wind_factors[height].count(stability) == 0 )
    return wind10m;
  return wind_factors[height][stability] * wind10m;
}

float WindCalc::gust(float wind10m, int stability, int height){
  if ( gust_factors.count(height) == 0 ||
       gust_factors[height].count(stability) == 0 )
    return wind10m;
  return gust_factors[height][stability] * wind10m;
}
