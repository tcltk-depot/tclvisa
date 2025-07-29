# tclvisa

This is fork of original tclvisa project [tclvisa](https://tclvisa.sourceforge.net/), adapted to work with modern
versions of tcl (tcl9).

## Introduction

tclvisa is a Tcl binding to VISA (Virtual Instrument Software Architecture) API.

VISA is a widely used I/O API in the test and measurement (T&M) industry for communicating with instruments from a
PC. VISA is an industry standard implemented by several T&M companies. Please refer to VISA documents and specifications
published on IVI Foundation home site: http://www.ivifoundation.org/specifications/default.aspx

tclvisa is a Tcl extension which follow Tcl Extension Architecture (TEA) conventions. Information about TEA can be found
at http://www.tcl.tk/doc/tea/

## Documentation

Documentation could be found here: 

## Compiling and installing tclvisa

### Introduction

tclvisa is a dynamic library written in C. Theoretically it can be complied and used on every platform which is
supported by Tcl, VISA and has C compiler.  tclvisa is linked dynamically into a Tcl shell or any application with
integrated Tcl interpreter. Basically one can issue "load" command to load library into the Tcl, but preferred way is to
use "package require" command.  Please refer to the TEA documentation for details about Tcl extensions and their usage:
http://www.tcl.tk/doc/tea/

### Compiling tclvisa

In order to compile tclvisa, you need the following:

- Tcl 8.6+ library and include files
- VISA library and include files
- C compiler

All source files are placed in "src/tclvisa" subdirectory. Entry point of the library is Tclvisa_Init function from
"tclvisa.c" file. Tcl looks for and calls this function when library is loaded.

tclvisa must be complied into dymanically linked library file (with .dll extension on Windows, .so on Linux etc).

### Installing tclvisa

Place the compiled library to the system library directory.


