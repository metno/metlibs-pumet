# $Id$

pkgname = puMet

include $(top_srcdir)/metlibs.mk

EXTRA_DIST += check-qbSymbols.def

lib_LTLIBRARIES=libpuMet.la

libpuMet_la_SOURCES = \
	gust.cc \
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
	visibility.cc \
	WindCalc.cc

libpuMet_la_CPPFLAGS = \
	${PUTOOLS_CPPFLAGS}

libpuMet_la_LDFLAGS = ${puMet_VERSION_INFO}
if ENABLE_SHARED_LIBRARY
libpuMet_la_LDFLAGS += \
	${PUTOOLS_LDFLAGS}

libpuMet_la_LIBADD = \
	${PUTOOLS_LIBS}
endif

pkg_includedir= ${includedir}/puMet

pkg_include_HEADERS = \
	addWeather.h \
	cloudGrp.h \
	ff2beaufort.h \
	gust.h \
	humidity.h \
	iceAccretion.h \
	miSymbol.h \
	paramet.h \
	ptStatistics.h \
	symbolMaker.h \
	usrTab.h \
	visibility.h \
	vision.h \
	weatherSpec.h \
	windProfile.h \
	WindCalc.h

check_PROGRAMS = \
	check-symbolMaker

check_symbolMaker_SOURCES = check-symbolMaker.cc
check_symbolMaker_CPPFLAGS = \
	-I$(top_srcdir) \
	-DPUMETDIR="\""$(top_srcdir)/puMet"\"" 

check_symbolMaker_LDFLAGS = $(CURL_LDFLAGS)
check_symbolMaker_LDADD = \
	libpuMet.la \
	$(top_builddir)/puTools/libpuTools.la $(CURL_LIBS)


TESTS = $(check_PROGRAMS)
