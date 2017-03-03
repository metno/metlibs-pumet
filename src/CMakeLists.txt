
INCLUDE_DIRECTORIES(${PC_METLIBS_INCLUDE_DIRS} ${BOOST_INCLUDE_DIRS})
ADD_DEFINITIONS(-W -Wall ${PC_METLIBS_CFLAGS_OTHER})
LINK_DIRECTORIES(${PC_METLIBS_LIBRARY_DIRS} ${BOOST_LIBRARY_DIRS})

########################################################################

SET(pumet_SOURCES
  gust.cc
  windProfile.cc
  vision.cc
  symbolMaker.cc
  miSymbol.cc
  addWeather.cc
  paramet.cc
  cloudGrp.cc
  weatherSpec.cc
  ptStatistics.cc
  usrTab.cc
  humidity.cc
  ff2beaufort.cc
  iceAccretion.cc
  visibility.cc
  WindCalc.cc
)

METNO_HEADERS (pumet_HEADERS pumet_SOURCES ".cc" ".h")
LIST(APPEND pumet_HEADERS
  puMetVersion.h
)

########################################################################

ADD_LIBRARY(pumet SHARED
  ${pumet_SOURCES}
)

SET_TARGET_PROPERTIES(pumet PROPERTIES
  VERSION ${PUMET_PVERSION_FULL}
  SOVERSION "${PUMET_VERSION_MAJOR}"
  OUTPUT_NAME "${lib_name}"
)

TARGET_LINK_LIBRARIES(pumet
  ${PC_METLIBS_LIBRARIES}
  ${BOOST_LIBRARIES}
)

INSTALL(TARGETS pumet
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

INSTALL(FILES ${pumet_HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/metlibs/puMet
)
