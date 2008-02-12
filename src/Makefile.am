lib_LTLIBRARIES=libpuMet.la

libpuMet_la_SOURCES= gust.cc \
	windProfile.cc \
	vision.cc \
	symbolMaker.cc \
	miSymbol.cc \
	addWeather.cc \
	paramet.cc \
	cloudGrp.cc \
	weatherSpec.cc \
	ptStatistics.cc \
	usrTab.cc \
	humidity.cc \
	ff2beaufort.cc \
	iceAccretion.cc \
	visibility.cc 

libpuMet_la_CXXFLAGS= ${puTools_CFLAGS} 
libpuMet_la_LDFLAGS= ${puTools_LIBS} ${VERSION_INFO}
INSTALL=install -p #tells install to preserve timestamp
DEFAULT_INCLUDES=-I${srcdir}/../include
