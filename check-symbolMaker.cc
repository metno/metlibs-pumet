/*
  libpuMet - Meteorological algorithms (Weather symbol generator etc.)

  $Id: addWeather.h 3273 2010-05-18 17:32:21Z dages $

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

#include <vector>
#include <map>
#include <set>
#include <string>
#include <fstream>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <check/tap>
#include "symbolMaker.h"

using namespace miutil;
using namespace tap;
using namespace std;

string pumetdir( PUMETDIR );

bool
readData( const string &filename,
          vector<paramet> &modelData,
          vector<miTime> &termins,
          float latitude=10 /*dummy latitude*/)
{
   set<miTime> times;
   map< int, map<miutil::miTime,float> > allParameters;
   string buf;
   vector<string> vals;
   ifstream fin( filename.c_str() );
   int par;
   miTime termin;
   float val;

   modelData.clear();
   termins.clear();

   if( ! fin ) {
      cerr << "failed to open file <" << filename << ">." << endl;
      return false;
   }

   while( getline( fin, buf ) ) {
      boost::split( vals, buf, boost::is_any_of(",") );

      if( vals.size() < 3 ) {
         cerr << "Grrrrrr" << endl;
         return false;
      }

      par = boost::lexical_cast<int>( vals[0] );
      termin = miTime( vals[1] );
      val = boost::lexical_cast<float>( vals[2] );

      times.insert( termin );
      allParameters[par][termin] = val;
   }

   for( map< int, map<miutil::miTime,float> >::const_iterator it = allParameters.begin();
        it != allParameters.end(); ++it ) {
      if( allParameters.count( it->first ) ) {
         paramet p;
         p.AddPara( it->first, allParameters[it->first], latitude );
         modelData.push_back( p );
      }
   }

   termins.reserve( times.size() );

   for( set<miTime>::const_iterator it=times.begin(); it != times.end(); ++it )
      termins.push_back( *it );

   return true;
}



class Common : public TestCase {

protected:
   void testCreateSymbol_( symbolMaker::Symboltype type ) {
      miSymbol symbol;
      std::ostringstream err;

      err << "createSymbol: symbolType: " << type;
      why = err.str();

      symbol = symbolMaker::createSymbol( type );

      if( symbol.index() != type ||
            symbolMaker::Symboltype( symbol.index() ) != type )
         throw std::logic_error( "FAILED: " + err.str() );

      err.str("");
      err << "symbol2type: symbolType: " << type ;
      why = err.str();
      symbolMaker::Symboltype symbolType = symbolMaker::symbol2type( symbol );

      if( symbolType != type )
         throw std::logic_error( "FAILED: " + err.str() );
   }

   void testCreateSymbol( symbolMaker::Symboltype type, bool expect=true ) {
      try{
         testCreateSymbol_( type );
         if( !expect ) {
            throw std::logic_error("FAILED: " + why );
         }
      }
      catch( const std::exception &ex ) {
         if( expect )
            throw std::logic_error("FAILED: " + why );
      }
   }
};

class test_createSymbol_and_symbol2type: public Common {

   TestOutcome run(void) {

      try {
         testCreateSymbol( symbolMaker::Sun );
         testCreateSymbol( symbolMaker::PartlyCloud );
         testCreateSymbol( symbolMaker::SleetSunThunder );
         testCreateSymbol( symbolMaker::SleetThunder  );
         testCreateSymbol( symbolMaker::ErrorSymbol );
      }
      catch( const std::exception &ex ) {
         why = ex.what();
         return fail;
      }

      why = "createSymbol and symbol2type.";
      return pass;
   }
};



class test_customNumber: public Common {

   TestOutcome run(void) {
      testCreateSymbol( symbolMaker::PartlyCloud );

      why = "miSymbol: customNumber and customName.";

      miSymbol symbol = symbolMaker::createSymbol( symbolMaker::PartlyCloud );

      if( symbol.customNumber( true ) != 3 ) {
         why = "FAILED: customNumber (light): PartlyCloud: expecting 3.";
         return fail;
      }

      if( symbol.customNumber( false ) != 17 ) {
         why = "FAILED: customNumber (dark): PartlyCloud: expecting 17.";
         return fail;
      }

      symbol.setLightStat(true);
      if( symbol.customName() != "delvis skyet" ) {
         why = "FAILED: customName: PartlyCloud: got '" + symbol.customName()+"' expecting 'delvis skyet'";
         return fail;
      }
      return pass;
   }
};

class test_computeWithoutStateOfAggregate: public Common {

   TestOutcome run(void) {
      symbolMaker sm;
      vector<paramet> modelData;
      vector<miTime> termins;
      string datafile(pumetdir + "/check-data1.dat");
      if( ! readData( datafile, modelData, termins ) ) {
         why = "FAILED - computeWithoutStateOfAggregate: Cant read data file <" + datafile +">.";
         return fail;
      }

      vector<miSymbol> symbols;
      symbols = sm.computeWithoutStateOfAggregate(
            modelData, termins, 1, 0, true );

      if( symbols.size() == 0 ) {
         why = "FAILED - computeWithoutStateOfAggregate: no symbols.";
         return fail;
      }

      for(vector<miSymbol>::iterator it=symbols.begin();
            it != symbols.end(); ++it )
         cout << it->getTime() << ": " << it->customNumber() << " : "
              << it->index() << " : " << it->customName() << endl;

      return pass;
   }
};


int
main(void)
{
   symbolMaker::readSymbols( pumetdir + "/check-qbSymbols.def");

   TestPlan tp;
   tp.comment( "symbolMaker");
   tp.add( new test_createSymbol_and_symbol2type() );
   tp.add( new test_customNumber() );
   tp.add( new test_computeWithoutStateOfAggregate() );
   tp.run();

   return tp.am_exit_code();
}
