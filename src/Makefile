# Makefile for met Library
# JS/LBS 1998-06-16
# last update JS 1/99 (make depends added)

LIBOBJ=	gust.o windProfile.o vision.o symbolMaker.o miSymbol.o \
	addWeather.o paramet.o cloudGrp.o weatherSpec.o \
	ptStatistics.o usrTab.o humidity.o ff2beaufort.o \
	iceAccretion.o visibility.o

LIBSRC=$(LIBOBJ:.o=.cc)

VPATH=$(LIBDIR)

.SUFFIXES: .cc .o

.cc.o:
	$(CXX) $(CCFLAGS) -c $< $(INCLUDE)

all: $(LIBDIR)/libpuMet.a

depends:
	rm -f $(DEPENDSFILE)
	echo '# Automatically-generated dependencies list:' > $(DEPENDSFILE)
	$(CXX) $(CCFLAGS) $(INCLUDE) -M $(LIBSRC) >> $(DEPENDSFILE)

$(LIBDIR)/libpuMet.a: $(LIBOBJ)
	$(AR) $(ARFLAGS) $@ $(LIBOBJ)

include $(DEPENDSFILE)
