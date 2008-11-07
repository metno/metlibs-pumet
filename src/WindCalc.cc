
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



int WindCalc::beaufort(float wind10m, miString & tekst){
  //conversion from wind (m/s) to beaufortvalue 
  int bf;
  float ms=wind10m;
  if (ms < 0) {
    bf=-1;
    tekst="Ugyldig";}
  else if ((ms >= 0) && (ms < 0.3)){
    bf = 0;
    tekst="Vindstille";}
  else if ((ms >= 0.3) && (ms < 1.6)) {
    bf = 1; 
    tekst = "Flau vind"; }
  else if ((ms >= 1.6) && (ms < 3.4)) {
    bf = 2; 
    tekst = "Svak vind"; }
  else if ((ms >= 3.4) && (ms < 5.5)) {
    bf = 3; 
    tekst = "Lett bris"; }
  else if ((ms >= 5.5) && (ms < 8.0)) {
    bf = 4; 
    tekst = "Laber bris"; }
  else if ((ms >= 8.0) && (ms < 10.8)) {
    bf = 5; 
    tekst = "Frisk bris"; }
  else if ((ms >= 10.8) && (ms < 13.9)) {
    bf = 6;
    tekst = "Liten kuling"; }
  else if ((ms >= 13.9) && (ms < 17.2)) {
  bf = 7;
  tekst = "Stiv kuling"; }
  else if ((ms >= 17.2) && (ms < 20.8)) {
    bf = 8; 
    tekst = "Sterk kuling"; }
  else if ((ms >= 20.8) && (ms < 24.5)) {
    bf = 9; 
    tekst = "Liten storm"; }
  else if ((ms >= 24.5) && (ms < 28.5)) {
    bf = 10;
    tekst = "Full storm"; }
  else if ((ms >= 28.5) && (ms < 32.6)) {
    bf = 11; 
    tekst = "Sterk storm"; }
  else if (ms >= 32.6) {
    bf = 12; 
    tekst = "Orkan";
  }
  return bf;
}
