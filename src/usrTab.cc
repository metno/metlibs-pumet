/*
  libpuMet - Meteorological algorithms (Weather symbol generator etc.)

  Copyright (C) 2006-2013 met.no

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

#include "usrTab.h"

#include <puTools/miStringFunctions.h>

#include <iostream>
#include <fstream>

using namespace std;


bool usrTab::create(const std::string& infile)
{
  ifstream inp;
  vector<std::string> tmpVec;

  if (infile.empty()) {
    cerr<<"No setup file specified!"<<endl;
    return false;
  }
  inp.open(infile.c_str(), ios::in);
  if (!inp){
    cerr<<infile<<" not found .... "<<endl;
    return false;
  }
  entity.clear();

  while(inp) {
    std::string tmpStr;
    getline(inp, tmpStr);

    if (miutil::contains(tmpStr, "#"))
      continue;

    if(tmpStr == mNull)
      continue;

    if (miutil::contains(tmpStr, "=")){
      tmpVec = miutil::split(tmpStr, "=");
      entity[tmpVec[0]] = tmpVec[1];
    }
  }
  inp.close();
  return true;
}


bool usrTab::exists(const std::string& m) const
{
  return entity.find(m) != entity.end();
}


miutil::miTime usrTab::dates(const std::string& index)
{
  miutil::miTime tmpTime = miutil::miTime::nowTime();
  std::string tmpStr = entity[index];

  if (!tmpStr.empty()) {
    const std::vector<std::string> tmpVec = miutil::split(tmpStr);

    if (tmpVec.size() > 2) {
        const int year = atoi(tmpVec[0].c_str());
        const int month = atoi(tmpVec[1].c_str());

        int hour;
        if (tmpVec.size() > 3)
            hour = atoi(tmpVec[3].c_str());
        else
            hour = 12;

        const int day = atoi(tmpVec[2].c_str());
        tmpTime.setTime(year, month, day, hour);
    }
  }

  return tmpTime;
}


std::string usrTab::content(const std::string& m) const
{
  const std::map<std::string, std::string>::const_iterator it = entity.find(m);
  if (it != entity.end())
    return it->second;
  else
    return std::string();
}


std::string usrTab::replace(const std::string& m) const
{
  const std::map<std::string, std::string>::const_iterator it = entity.find(m);
  if (it != entity.end())
    return it->second;
  else
    return m;
}


std::string usrTab::file(const std::string& m) const
{
  return content("WORKDIR") + content(m);
}


void usrTab::makeByHand(const std::string& m, const std::string& requ)
{
  entity[m] = requ;
}


int usrTab::numInt(const std::string& m) const
{
  return atoi(content(m).c_str());
}


float usrTab::numFloat(const std::string& m) const
{
  return atof(content(m).c_str());
}


map<std::string,std::string> usrTab::selected(const vector<std::string>& sel)
{
  map<std::string,std::string> tmp;
  for (vector<std::string>::const_iterator sit =sel.begin(); sit != sel.end(); ++sit) {
    const std::map<std::string, std::string>::const_iterator it = entity.find(*sit);
    if (it != entity.end())
      tmp[*sit] = it->second;
  }
  return tmp;
}
