# $Id: Makefile.am 2786 2009-09-23 11:39:02Z dages $

pkgname = puMet

include $(top_srcdir)/metlibs.mk

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

libpuMet_la_LDFLAGS = \
	${VERSION_INFO} \
	${PUTOOLS_LDFLAGS}

libpuMet_la_LIBADD = \
	${PUTOOLS_LIBS}

pkg_includedir= ${prefix}/include/puMet

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

