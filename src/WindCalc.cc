#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "WindCalc.h"
#include <puTools/miStringFunctions.h>
#include <iostream>
#include <fstream>

using namespace std;
using namespace miutil;

bool WindCalc::readFactors(const std::string& filename)
{
  cerr << "++ READ wind-information from:" << filename << endl;

  // first clean up
  wind_factors.clear();
  gust_factors.clear();

  ifstream f(filename.c_str());
  if (!f) {
    cerr << "ERROR opening file:" << filename << endl;
    return false;
  }

  std::string buf;
  std::string syntax = "(Stable|Neutral|Instable)_heightinmetre_(Gust|Mean) = floatvalue";

  while (getline(f, buf)){
    miutil::trim(buf);
    if (buf.length()==0)
      continue;
    if (buf[0]=='#')
      continue;

    const vector<std::string> vs = miutil::split(buf, "=");

    if ( vs.size() != 2 ) {
      cerr << "Warning, illegal line in wind-information file:"
           << buf << endl
           << " Syntax is:" << syntax << endl;
      continue;
    }
    if (!miutil::is_number(vs[1])) {
      cerr << "Warning, illegal entry in wind-information file:"
           << buf << endl
           << " Syntax is:" << syntax << endl;
      continue;
    }
    float value = miutil::to_float(vs[1]);

    const vector<std::string> vvs = miutil::split(vs[0], "_");
    if (vvs.size() != 3) {
      cerr << "Warning, illegal entry in wind-information file:"
           << buf << endl
           << " Syntax is:" << syntax << endl;
      continue;
    }

    int stability = 0;
    if (miutil::to_lower(vvs[0]) == "stable") {
      stability = 1;
    } else if (miutil::to_lower(vvs[0]) == "instable") {
      stability = -1;
    }

    if ( !miutil::is_int(vvs[1])) {
      cerr << "Warning, illegal entry in wind-information file:"
           << buf << endl
           << " Syntax is:" << syntax << endl;
      continue;
    }
    int height = miutil::to_int(vvs[1]);

    if (miutil::to_lower(vvs[2]) == "gust") {
      gust_factors[height][stability] = value;
    } else if (miutil::to_lower(vvs[2]) == "mean") {
      wind_factors[height][stability] = value;
    } else {
      cerr << "Warning, expected GUST or MEAN in:"
           << buf << endl
           << " Syntax is:" << syntax << endl;
      continue;
    }
  }

  return true;
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

int WindCalc::beaufort(float wind10m, std::string& tekst)
{
  //conversion from wind (m/s) to beaufortvalue
  int bf;
  float ms=wind10m;
  if (ms < 0) {
    bf=-1;
    tekst="Ugyldig";}
  else if ((ms >= 0) && (ms < 0.25)){
    bf = 0;
    tekst="Vindstille";}
  else if ((ms >= 0.25) && (ms < 1.55)) {
    bf = 1;
    tekst = "Flau vind"; }
  else if ((ms >= 1.55) && (ms < 3.35)) {
    bf = 2;
    tekst = "Svak vind"; }
  else if ((ms >= 3.35) && (ms < 5.45)) {
    bf = 3;
    tekst = "Lett bris"; }
  else if ((ms >= 5.45) && (ms < 7.95)) {
    bf = 4;
    tekst = "Laber bris"; }
  else if ((ms >= 7.95) && (ms < 10.75)) {
    bf = 5;
    tekst = "Frisk bris"; }
  else if ((ms >= 10.75) && (ms < 13.85)) {
    bf = 6;
    tekst = "Liten kuling"; }
  else if ((ms >= 13.85) && (ms < 17.15)) {
  bf = 7;
  tekst = "Stiv kuling"; }
  else if ((ms >= 17.15) && (ms < 20.75)) {
    bf = 8;
    tekst = "Sterk kuling"; }
  else if ((ms >= 20.75) && (ms < 24.45)) {
    bf = 9;
    tekst = "Liten storm"; }
  else if ((ms >= 24.45) && (ms < 28.45)) {
    bf = 10;
    tekst = "Full storm"; }
  else if ((ms >= 28.45) && (ms < 32.55)) {
    bf = 11;
    tekst = "Sterk storm"; }
  else if (ms >= 32.55) {
    bf = 12;
    tekst = "Orkan";
  }
  return bf;
}
