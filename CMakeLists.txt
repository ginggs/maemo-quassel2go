# This is the cmake-based build system for Quassel IRC.
#
# You may pass various options to cmake:
# -DWANT_(CORE|QTCLIENT|MOBILECLIENT|MONO)=(ON|OFF)
#                        : select binaries to build
# -DWITH_OPENSSL=OFF     : Disable OpenSSL support
# -DWITH_DBUS=OFF        : Disable D-Bus support (dbus notifications)
# -DWITH_WEBKIT=OFF      : Disable WebKit support (link previews)
# -DWITH_PHONON=OFF      : Disable Phonon support (audio notifications)
# -DWITH_LIBINDICATE=OFF : Disable libindicate support (Ayatana notifications)
# -DWITH_KDE=ON          : Enable KDE4 support
# -DWITH_CRYPT=OFF       : Disable encryption support
# -DWITH_OXYGEN=(ON|OFF) : Whether to install Oxygen icons (default: yes, unless KDE > 4.3.0 is present and enabled)
#
# -DEMBED_DATA=ON        : Embed all data files in icons the binary, rather than installing them separately
#
# -DQT=/path/to/qt       : Choose a Qt4 installation to use instead of the system Qt4
# -DSTATIC=ON            : Enable static building of Quassel. Use with care.
# -DDEPLOY=ON            : Mac OS X only. Use only for creating Quassel Packages!
#
# NOTE: You should remove CMakeCache.txt if you plan to change any of these values!

project(QuasselIRC)

# cmake 2.6.2 is required for KDE >=4.2 and should be widespread enough now
cmake_minimum_required(VERSION 2.6.2 FATAL_ERROR)

if(COMMAND cmake_policy)
   cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# Use our own (well, and KDE's) version of some modules
# In particular cmake's own FindQt4 and FindOpenSSL are quite buggy
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
include(QuasselMacros)

# Various options and variables that can be set on the command line
option(WANT_CORE     "Build the core (server) binary"           ON)
option(WANT_QTCLIENT "Build the Qt4 GUI client binary"          ON)
option(WANT_MOBILECLIENT "Build the Qt4 mobile GUI client binary" OFF)
option(WANT_MONO     "Build the monolithic (all-in-one) binary" ON)

option(WITH_OPENSSL  "Enable OpenSSL support if present on the system"	ON)
option(WITH_DBUS     "Enable D-Bus support if present on the system"	ON)
option(WITH_WEBKIT   "Enable WebKit support (for link previews)"	ON)
option(WITH_PHONON   "Enable Phonon support (for audio notifications)"	ON)
option(WITH_LIBINDICATE "Enable Ayatana notification support"           ON)
option(WITH_KDE      "Enable KDE4 integration"				OFF)
option(WITH_CRYPT    "Enable encryption support if present on system"	ON)

# We use icon paths from KDE 4.3.x, which are partially invalid on older and possibly
# even on newer KDE versions. Do not disable this unless you are sure that your Quassel will
# run on a matching KDE version only.
set(WITH_OXYGEN AUTO CACHE STRING "Install Oxygen icons (default is \"AUTO\" to install when KDE 4.3 or later is present")

option(STATIC        "Enable static building (might not be portable)" OFF)

if(APPLE)
  option(DEPLOY        "Mac OS X only! Adds required libs to bundle resources and create a dmg. Note: requires Qt to be built with 10.4u SDK" OFF)
endif(APPLE)

# Default to embedding data in the static case
if(STATIC OR WIN32)
  set(EMBED_DEFAULT ON)
else(STATIC OR WIN32)
  set(EMBED_DEFAULT ON) # should be OFF as soon as everything works
endif(STATIC OR WIN32)

option(EMBED_DATA    "Embed all data files in the binary (rather than installing them separately)"   ${EMBED_DEFAULT})

set(QT "" CACHE STRING "Path to a Qt installation to use instead of the system Qt (e.g. for static builds)")

# Some settings imply others
if(STATIC)
  add_definitions(-DSTATIC)
  set(WITH_KDE OFF CACHE BOOL "Static building with KDE is not supported")
endif(STATIC)

if(WIN32)
  # We don't support separately installed resources yet on Win32
  set(EMBED_DATA ON)
endif(WIN32)

# For static builds, arbitrary extra libs might need to be linked
# Define a comma-separated list here
# e.g. for pgsql, we need -DLINK_EXTRA=pq;crypt
set(LINK_EXTRA "" CACHE STRING "Semicolon-separated list of libraries to be linked")
if(LINK_EXTRA)
  string(REPLACE "," ";" LINK_EXTRA ${LINK_EXTRA})
  link_libraries(${LINK_EXTRA})
endif(LINK_EXTRA)

# Build Type
# We need to make sure it's not empty
# Supported: Release, RelWithDebugInfo, Debug, Debugfull

# On WIN32, only Release seems to work correctly (?)
if(WIN32)
  set(DEFAULT_BUILD_TYPE "Release")
else(WIN32)
  set(DEFAULT_BUILD_TYPE "RelWithDebugInfo")
endif(WIN32)

set(CMAKE_BUILD_TYPE ${DEFAULT_BUILD_TYPE} CACHE STRING "CMake Build Type")
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE ${DEFAULT_BUILD_TYPE} CACHE STRING "CMake Build Type" FORCE)
endif(NOT CMAKE_BUILD_TYPE)

# Enable various flags on gcc
if(CMAKE_COMPILER_IS_GNUCXX)
  # Let's just hope that all gccs support these options and skip the tests...
  # -fno-strict-aliasing is needed apparently for Qt < 4.6
  set(CMAKE_CXX_FLAGS                  "${CMAKE_CXX_FLAGS} -ansi -Wall -Wextra -Wnon-virtual-dtor -fno-strict-aliasing")
  set(CMAKE_CXX_FLAGS_RELEASE          "-O2 ${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS_RELWITHDEBUGINFO "-g -O2 ${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS_DEBUG            "-g -ggdb -fno-reorder-blocks -fno-schedule-insns -fno-inline ${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS_DEBUGFULL        "-g3 ${CMAKE_CXX_FLAGS_DEBUG}")
endif(CMAKE_COMPILER_IS_GNUCXX)

string(TOUPPER ${CMAKE_BUILD_TYPE} upper_build_type)
if(upper_build_type STREQUAL "RELEASE" OR upper_build_type STREQUAL "RELWITHDEBUGINFO")
  add_definitions(-DNDEBUG -DQT_NO_DEBUG)
else(upper_build_type STREQUAL "RELEASE" OR upper_build_type STREQUAL "RELWITHDEBUGINFO")
  set(DEBUG 1)
endif(upper_build_type STREQUAL "RELEASE" OR upper_build_type STREQUAL "RELWITHDEBUGINFO")

if(APPLE AND DEPLOY)
  set(CMAKE_OSX_ARCHITECTURES "i386;ppc")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmacosx-version-min=10.4")
  set(CMAKE_OSX_SYSROOT "/Developer/SDKs/MacOSX10.4u.sdk/")
  add_definitions(-DMAC_10_4_SDK)
endif(APPLE AND DEPLOY)

# Simplify checks
if(WANT_MONO OR WANT_QTCLIENT OR WANT_MOBILECLIENT)
  set(BUILD_GUI true)
endif(WANT_MONO OR WANT_QTCLIENT OR WANT_MOBILECLIENT)
if(WANT_MONO OR WANT_CORE)
  set(BUILD_CORE true)
endif(WANT_MONO OR WANT_CORE)

# Dependencies
##############

# GUI stuff needs some new features
if(BUILD_GUI)
  set(QT_MIN_VERSION "4.6.0")
else(BUILD_GUI)
  set(QT_MIN_VERSION "4.4.0")
endif(BUILD_GUI)

# Execinfo is needed for generating backtraces
find_package(ExecInfo)
if(EXECINFO_FOUND)
  add_definitions(-DHAVE_EXECINFO)
  include_directories(${EXECINFO_INCLUDES})
  link_libraries(${EXECINFO_LIBRARIES})
endif(EXECINFO_FOUND)

# Select a Qt installation here, if you don't want to use system Qt
if(QT)
  # FindQt4 will look for the qmake binary in $PATH, so we just prepend the Qt dir
  set(ENV{PATH} ${QT}/bin:$ENV{PATH})
endif(QT)

# Now that we have the correct $PATH, lets find Qt!
find_package(Qt4 REQUIRED)

set(QT_DONT_USE_QTGUI 1)
include(${QT_USE_FILE})
include_directories(${QT_INCLUDES})

# PkgConfig isn't strictly required.
# However, some optional deps might not be found if it's not present, so warn!
find_package(PkgConfig)
if(NOT PKG_CONFIG_FOUND)
  message(STATUS "WARNING: PkgConfig not available! Some dependencies for optional features might not be found.")
  message(STATUS "         Affected features might include encryption support, DBus menus and Ayatana notifications.")
endif(NOT PKG_CONFIG_FOUND)

# Setup OpenSSL
# We don't link to or include OpenSSL ourselves, but use exclusively the Qt API.
# Thus, we simply check if OpenSSL support is present in Qt's config and enable our
# own SSL stuff in this case. Qt should care for adding what it needs itself.
if(WITH_OPENSSL)
  if(QT_QCONFIG MATCHES "openssl")
    message(STATUS "Found OpenSSL support in Qt, enabling SSL")
    add_definitions(-DHAVE_SSL)
    set(HAVE_SSL true)
  else(QT_QCONFIG MATCHES "openssl")
    message(STATUS "No OpenSSL support found in Qt, disabling SSL")
    add_definitions(-DQT_NO_OPENSSL)
  endif(QT_QCONFIG MATCHES "openssl")
else(WITH_OPENSSL)
  message(STATUS "Not enabling OpenSSL support")
endif(WITH_OPENSSL)

# Check for GUI specific stuff
if(BUILD_GUI)

  # Setup D-Bus support
  if(WITH_DBUS)
    if(QT_QTDBUS_FOUND)
      message(STATUS "Found QtDBus, enabling D-Bus support")
      add_definitions(-DHAVE_DBUS)
      set(CLIENT_QT4_VARS ${CLIENT_QT4_VARS} DBUS)
      set(CLIENT_COMPILE_FLAGS "${CLIENT_COMPILE_FLAGS} -DQT_DBUS_LIB")
      set(HAVE_DBUS true)

      # check if we have dbusmenu as well
      find_package(DBusMenuQt)
      if(DBUSMENUQT_FOUND)
        message(STATUS "Enabling support for exporting the tray menu via D-Bus")
        add_definitions(-DHAVE_DBUSMENU)
        include_directories(${DBUSMENUQT_INCLUDE_DIR})
        set(CLIENT_LIBRARIES ${CLIENT_LIBRARIES} ${DBUSMENUQT_LIBRARIES})
        set(CLIENT_COMPILE_FLAGS "${CLIENT_COMPILE_FLAGS} ${DBUSMENUQT_DEFINITIONS}")
      else(DBUSMENUQT_FOUND)
        message(STATUS "Disabling support for exporting the tray menu via D-Bus")
      endif(DBUSMENUQT_FOUND)

    else(QT_QTDBUS_FOUND)
      message(STATUS "QtDBus not found, disabling D-Bus support")
    endif(QT_QTDBUS_FOUND)
  else(WITH_DBUS)
    message(STATUS "Not enabling D-Bus support")
  endif(WITH_DBUS)

  # Setup QtWebKit support
  if(WITH_WEBKIT)
    if(QT_QTWEBKIT_FOUND)
      message(STATUS "Found QtWebKit, enabling WebKit support")
      add_definitions(-DHAVE_WEBKIT)
      set(CLIENT_QT4_VARS ${CLIENT_QT4_VARS} WEBKIT XMLPATTERNS)
      set(CLIENT_COMPILE_FLAGS "${CLIENT_COMPILE_FLAGS} -DQT_WEBKIT_LIB -DQT_XMLPATTERNS_LIB")
      set(HAVE_WEBKIT true)
    else(QT_QTWEBKIT_FOUND)
      message(STATUS "QtWebKit not found, disabling WebKit support")
    endif(QT_QTWEBKIT_FOUND)
  else(WITH_WEBKIT)
    message(STATUS "Not enabling WebKit support")
  endif(WITH_WEBKIT)

  # Setup KDE4 support
  if(WITH_KDE)
    find_package(KDE4)
    if(KDE4_FOUND)
      message(STATUS "Enabling KDE4 integration")
      include_directories(${KDE4_INCLUDES})
      add_definitions(-DHAVE_KDE ${KDE4_DEFINITIONS})
      set(HAVE_KDE 1)
      set(CLIENT_LIBRARIES ${CLIENT_LIBRARIES} ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBRARY} ${KDE4_SOLID_LIBS} knotifyconfig)
      # We always use external icons for KDE4 support, since we use its iconloader rather than our own
      set(EMBED_DATA OFF)
    else(KDE4_FOUND)
      message(STATUS "KDE4 not found, disabling KDE integration")
    endif(KDE4_FOUND)
  else(WITH_KDE)
    message(STATUS "Not enabling KDE4 integration")
  endif(WITH_KDE)

  # Setup Phonon support - we only need this if we don't have or want KDE4
  if(NOT HAVE_KDE)
    if(WITH_PHONON)
      find_package(Phonon)
      if(PHONON_FOUND)
        message(STATUS "Enabling Phonon support")
        add_definitions(-DHAVE_PHONON)
        include_directories(${PHONON_INCLUDES})
        set(CLIENT_LIBRARIES ${CLIENT_LIBRARIES} ${PHONON_LIBS})
        set(HAVE_PHONON true)
      else(PHONON_FOUND)
        message(STATUS "Phonon not found, disabling audio notifications")
      endif(PHONON_FOUND)
    else(WITH_PHONON)
      message(STATUS "Not enabling Phonon support")
    endif(WITH_PHONON)
  endif(NOT HAVE_KDE)

  # Setup libindicate-qt support
  if(WITH_LIBINDICATE)
    pkg_check_modules(INDICATEQT QUIET indicate-qt>=0.2.1)
    if(INDICATEQT_FOUND)
      message(STATUS "Enabling Ayatana notification support")
      set(HAVE_INDICATEQT true)
      add_definitions(-DHAVE_INDICATEQT)
      link_directories(${INDICATEQT_LIBRARY_DIRS})
      set(CLIENT_LIBRARIES ${CLIENT_LIBRARIES} ${INDICATEQT_LIBRARIES})
    else(INDICATEQT_FOUND)
      message(STATUS "Disabling Ayatana notification support")
    endif(INDICATEQT_FOUND)
  else(WITH_LIBINDICATE)
    message(STATUS "Not enabling Ayatana notification support")
    # We don't want to link against it even if another package has found it
    set(INDICATEQT_LIBRARIES "")
  endif(WITH_LIBINDICATE)

  # Bearer management, QML
  if(WANT_MOBILECLIENT)
    # bearer
    if(QT_VERSION_MAJOR EQUAL 4 AND QT_VERSION_MINOR LESS 7)
      find_package(QtMobility REQUIRED)
      if(QT_MOBILITY_FOUND)
        message(STATUS "Enabling QtMobility support for Bearer management.")
        add_definitions(-DHAVE_QT_BEARER)
        set(HAVE_QT_BEARER 1)
      endif(QT_MOBILITY_FOUND)
    else(QT_VERSION_MAJOR EQUAL 4 AND QT_VERSION_MINOR LESS 7)
      message(STATUS "Using Bearer management which comes with Qt >= 4.7.")
      add_definitions(-DHAVE_QT_BEARER)
      set(HAVE_QT_BEARER 1)
    endif(QT_VERSION_MAJOR EQUAL 4 AND QT_VERSION_MINOR LESS 7)

    # hildon notifications (glib+gtk, ugh...)
    find_package(Libnotify REQUIRED)
    if(LIBNOTIFY_FOUND)
      message(STATUS "Enabling Libnotify notifications support")
      include_directories(${LIBNOTIFY_INCLUDE_DIR} ${GLIB2_INCLUDE_DIR} ${GTK2_INCLUDE_DIRS})
      set(CLIENT_LIBRARIES ${CLIENT_LIBRARIES} ${LIBNOTIFY_LIBRARIES} ${GLIB2_LIBRARIES} ${GTK2_LIBRARIES})
    endif(LIBNOTIFY_FOUND)

    # QML
    set(QT_MIN_VERSION "4.7.0")
    if(QT_QTOPENGL_FOUND)
      message(STATUS "Found QtOpengl, enabling the QML-based Mobile UI")
      add_definitions(-DHAVE_OPENGL)
      set(CLIENT_QT4_VARS ${CLIENT_QT4_VARS} OPENGL)
       set(CLIENT_COMPILE_FLAGS "${CLIENT_COMPILE_FLAGS} -DQT_OPENGL_LIB")
      set(HAVE_OPENGL true)
    else(QT_QTOPENGL_FOUND)
        message (FATAL_ERROR "QtOpengl module is required for QML based mobile UI.")
    endif(QT_QTOPENGL_FOUND)
    if(QT_QTDECLARATIVE_FOUND)
      message(STATUS "Found QtDeclarative, enabling the QML-based Mobile UI")
      add_definitions(-DHAVE_QML)
      set(CLIENT_QT4_VARS ${CLIENT_QT4_VARS} DECLARATIVE)
       set(CLIENT_COMPILE_FLAGS "${CLIENT_COMPILE_FLAGS} -DQT_DECLARATIVE_LIB")
      set(HAVE_QML true)
    else(QT_QTDECLARATIVE_FOUND)
        message(FATAL_ERROR "QtDeclarative not found, unable to build mobile UI")
    endif(QT_QTDECLARATIVE_FOUND)
  endif(WANT_MOBILECLIENT)

endif(BUILD_GUI)

# Core-only deps
if(BUILD_CORE)

  # Setup encryption support
  if(WITH_CRYPT)
    find_package(QCA2)
    if(QCA2_FOUND)
      message(STATUS "Found QCA2, enabling encryption support")
      add_definitions(-DHAVE_QCA2)
      set(LINK_QCA2 QCA2)
      set(HAVE_QCA2 true)
      set(MOC_DEFINES ${MOC_DEFINES} -DHAVE_QCA2)
    else(QCA2_FOUND)
      message(STATUS "QCA2 not found, disabling encryption support")
    endif(QCA2_FOUND)
  else(WITH_CRYPT)
    message(STATUS "Not enabling encryption support")
  endif(WITH_CRYPT)

endif(BUILD_CORE)

# needed to compile with mingw without kde
if(MINGW AND NOT HAVE_KDE)
    add_definitions(-D_WIN32_WINNT=0x0500)
    message(STATUS "Added _WIN32_WINNT=0x0500 definition for MinGW")
# workaround for bug in mingw gcc 4.0
    add_definitions(-U__STRICT_ANSI__)
endif(MINGW AND NOT HAVE_KDE)

# Now set up install locations; those are set by KDE if integration is enabled
if(NOT HAVE_KDE)
  if(WIN32)
    set(BIN_INSTALL_DIR ${CMAKE_INSTALL_PREFIX} CACHE FILEPATH "Install path for binaries")
    set(DATA_INSTALL_DIR $ENV{APPDATA}/quassel-irc.org/share/apps CACHE FILEPATH "Install path for data files")
    set(ICON_INSTALL_DIR $ENV{APPDATA}/quassel-irc.org/share/icons CACHE FILEPATH "Global icon install path")
    set(XDG_APPS_INSTALL_DIR $ENV{APPDATA}/quassel-irc.org/share/applications CACHE FILEPATH "Install path for .desktop files")
  else(WIN32)
    set(BIN_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/bin CACHE FILEPATH "Install path for binaries")
    set(DATA_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/share/apps CACHE FILEPATH "Install path for data files")
    set(ICON_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/share/icons CACHE FILEPATH "Global icon install path")
    set(XDG_APPS_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/share/applications CACHE FILEPATH "Install path for .desktop files")
  endif(WIN32)
endif(NOT HAVE_KDE)

if(EMBED_DATA)
  message(STATUS "Embedding data files into the binary")
else(EMBED_DATA)
  message(STATUS "Installing data files separately")
endif(EMBED_DATA)

# RPATH needs to be set correctly
# Do this down here, since otherwise KDE wants to handle it itself, and fails
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH 1)
set(CMAKE_BUILD_WITH_INSTALL_RPATH 1)

# Set global buildflags
# This is very much non-portable, so don't use -DSTATIC until you know what
# you do.
if(STATIC AND CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "-static-libgcc ${CMAKE_CXX_FLAGS}")
  link_directories(${CMAKE_BINARY_DIR}/staticlibs) # override dynamic libs
  if(HAVE_SSL)
    set(QUASSEL_SSL_LIBRARIES ssl crypto)  # these miss in static builds
  endif(HAVE_SSL)
endif(STATIC AND CMAKE_COMPILER_IS_GNUCXX)

if(WIN32)
  link_libraries(imm32 winmm dbghelp Secur32)  # missing by default :/
  if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBUGINFO "/debug /INCREMENTAL:YES /NODEFAULTLIB:libcmt /DEFAULTLIB:msvcrt")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBUGINFO}")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUGFULL "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBUGINFO}")
    link_libraries(Version dwmapi shlwapi)
  endif(MSVC)
  if(HAVE_SSL AND STATIC)
     find_package(OpenSSL REQUIRED)
     link_libraries(${OPENSSL_LIBRARIES} ${OPENSSL_EAY_LIBRARIES})
  endif(HAVE_SSL AND STATIC)
endif(WIN32)

if(HAVE_INDICATEQT)
  add_definitions(-DXDG_APPS_INSTALL_DIR=${XDG_APPS_INSTALL_DIR})
endif(HAVE_INDICATEQT)

# We need to create a version.gen
# For this, we create our genversion binary and make sure it is run every time.

# things get slightly more tricky when cross compiling because the
# cross compiled genversion binary can't be used. therefore, import
# the executable targets from a file, specified by:
# -DIMPORT_EXECUTABLES=<path-to-native-build>/ImportExecutables.cmake
IF(CMAKE_CROSSCOMPILING OR FORCE_CROSSCOMPILING)
  message(STATUS "Activating cross compile and importing native executables from '${IMPORT_EXECUTABLES}'")
  SET(IMPORT_EXECUTABLES "IMPORTFILE-NOTFOUND" CACHE FILEPATH "Point it to the export file from a native build")
  INCLUDE(${IMPORT_EXECUTABLES})
ENDIF(CMAKE_CROSSCOMPILING OR FORCE_CROSSCOMPILING)
# only build the generator if not crosscompiling
IF(NOT (CMAKE_CROSSCOMPILING OR FORCE_CROSSCOMPILING))
  add_executable(genversion ${CMAKE_SOURCE_DIR}/src/common/genversion.cpp)
  target_link_libraries(genversion ${QT_LIBRARIES} ${QT_CORE_LIB_DEPENDENCIES})
ENDIF(NOT (CMAKE_CROSSCOMPILING OR FORCE_CROSSCOMPILING))
# export the generator target to a file, so it can be imported (see above) by another build
# the IF() is not necessary, but makes the intention clearer
IF(NOT (CMAKE_CROSSCOMPILING OR FORCE_CROSSCOMPILING))
  EXPORT(TARGETS genversion FILE ${CMAKE_BINARY_DIR}/ImportExecutables.cmake )
ENDIF(NOT (CMAKE_CROSSCOMPILING OR FORCE_CROSSCOMPILING))
# then use the target name as COMMAND, CMake >= 2.6 knows how to
# handle executables imported during cross compile
ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_BINARY_DIR}/src/version.gen
                   COMMAND genversion ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}/src/version.gen
                   DEPENDS ${CMAKE_SOURCE_DIR}/.git )
# the custom command is needed because only targets can be referenced
# from within src/CMakeLists.txt
ADD_CUSTOM_TARGET(genversion_run ALL DEPENDS ${CMAKE_BINARY_DIR}/src/version.gen)

# These variables will be added to the main targets (CORE, QTCLIENT, MOBILECLIENT, MONO)
set(COMMON_DEPS ${RC_WIN32})
set(CORE_DEPS )
set(CLIENT_DEPS )

# Add needed subdirs - the order is important, since src needs some vars set by other dirs
add_subdirectory(data)
add_subdirectory(icons)
add_subdirectory(pics)
add_subdirectory(po)
add_subdirectory(src)
