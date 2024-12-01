CMake based build support for APR on Microsoft Windows

Status
------

This build support is currently intended only for Microsoft Windows.

Prerequisites
-------------

The following tools must be in PATH:

* cmake, version 3.5 or later
* If using a command-line compiler: compiler and linker and related tools
  (Refer to the cmake documentation for more information.)

Additional support libraries allow optional features of APR to be enabled:

* Expat
* Iconv
* Libxml2
* SQlite3
* OpenSSL

How to build
------------

1. cd to a clean directory for building (i.e., don't build in your
   source tree)

2. Some cmake backends may want your compile tools in PATH.  (Hint: "Visual
   Studio Command Prompt")

3. cmake -G "some backend, like 'Ninja' or 'NMake Makefiles'"
     -DCMAKE_INSTALL_PREFIX=d:/path/to/aprinst
     -DAPR-specific-flags
     d:/path/to/aprsource

   Alternately, use cmake-gui and update settings in the GUI.

   APR feature flags:

       Exactly one of APU_USE_EXPAT and APU_USE_LIBXML2 must be specified.

       APU_USE_EXPAT          Use Expat as the underlying XML implementation
                              Default: ON
       APU_USE_LIBXML2        Use libxml2 as the underlying XML
                              implementation
                              Default: OFF
       APR_INSTALL_PRIVATE_H  Install extra .h files which are required when
                              building httpd and Subversion but which aren't
                              intended for use by applications.
                              Default: OFF
       APU_HAVE_CRYPTO        Build crypt support (only the OpenSSL
                              implementation is currently supported)
                              Default: OFF
       APU_HAVE_ODBC          Build ODBC DBD driver
                              Default: ON
       APU_HAVE_SQLITE3       Build SQLite3 DBD driver
                              Default: OFF
       APU_HAVE_PGSQL         Build PostgreSQL DBD driver
                              Default: OFF
       APR_HAVE_IPV6          Enable IPv6 support
                              Default: ON
       APR_BUILD_TESTAPR      Build APR test suite
                              Default: OFF
       APR_MODULAR_DSO        Use DSO build of modular components. Otherwise modular
                              components will be statically linked.
                              Default: ON
       APR_POOL_DEBUG         Turn on pools debugging
                              Default: OFF
       INSTALL_PDB            Install .pdb files if generated.
                              Default: ON

   CMAKE_C_FLAGS_RELEASE, _DEBUG, _RELWITHDEBINFO, _MINSIZEREL

   CMAKE_BUILD_TYPE

       For NMake Makefiles the choices are at least DEBUG, RELEASE,
       RELWITHDEBINFO, and MINSIZEREL
       Other backends make have other selections.

4. build using chosen backend (e.g., "nmake install")

Tested generators
-----------------

1. Ninja

This has been tested successfully with the following:

1. Visual Studio 2019 and Visual Studio 2022

Known Bugs and Limitations
--------------------------

* If include/apr.h or other generated files have been created in the source
  directory by another build system, they will be used unexpectedly and
  cause the build to fail.
* Options should be provided for remaining features, along with finding any
  necessary libraries
  + DBM:
    . APU_HAVE_GDBM
    . APU_HAVE_NDBM
    . APU_HAVE_DB
  + DBD:
    . APU_HAVE_MYSQL
    . APU_HAVE_SQLITE2
    . APU_HAVE_ORACLE
  + CRYPTO:
    . APU_HAVE_NSS
  + APU_HAVE_ICONV

Generally:

* Any feedback you can provide on your experiences with this build will be
  helpful.
