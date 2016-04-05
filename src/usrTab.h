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


#ifndef _usrTab_h
#define _usrTab_h

#include <puTools/miTime.h>

#include <map>
#include <string>
#include <vector>

class usrTab {
private:
  std::map<std::string, std::string> entity;
  std::string mNull;

public:
  usrTab() {}

  bool create(const std::string&);

  bool exists(const std::string& m) const;

  miutil::miTime dates(const std::string&);

  std::string content(const std::string& m) const;

  std::string replace(const std::string& m) const;

  std::string file(const std::string& m) const;

  void makeByHand(const std::string& m, const std::string& requ);

  int numInt(const std::string& m) const;

  float numFloat(const std::string& m) const;

  std::string operator[] (const std::string &m) const
    { return content(m); }

  std::map<std::string,std::string> selected(const std::vector<std::string> & );
};

#endif
