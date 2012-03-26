#!/usr/bin/tclsh

##########################################################
#
# last-error.tcl --
#
# This file is part of tclvisa library.
#
# Copyright (c) 2011 Andrey V. Nakin <andrey.nakin@gmail.com>
# All rights reserved.
#
# See the file "COPYING" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
##########################################################

##########################################################
#
# This file  demonstrates usage of visa::last-error command
# We open an instrument, read from it and check error status.
#
# Usage: last-error.tcl [{VISA INSTR address}]
#
##########################################################

package require tclvisa

set DEFAULT_ADDR "ASRL1::INSTR"

##########################################################
# ENTRY POINT
##########################################################

if { [llength $argv] >= 1 } {
  set visaAddr [lindex $argv 0]
} else {
  set visaAddr $DEFAULT_ADDR
  puts stderr "Device address is not specified, use `$visaAddr` by default"
}

if { [catch { set rm [visa::open-default-rm] }] } {
  puts stderr "Error opening resource manager";
  exit
}

set addr ASRL1
if { [catch { set vi [visa::open $rm $visaAddr] } ] } {
  set err [visa::last-error $rm]
  puts stderr "Error opening instrument $visaAddr";
  puts stderr "Code: [lindex $err 0]"
  puts stderr "Char code: [lindex $err 1]"
  puts stderr "Description: [lindex $err 2]"
  exit
}

# read from device
set ans [gets $vi]

if { $ans == "" } {
  # Either timeout error or empty device response
  set err [visa::last-error $vi]
  if { [lindex $err 0] == $visa::ERROR_TMO } {
    puts stderr "Error reading from instrument $visaAddr"
    puts stderr "Code: [lindex $err 0]"
    puts stderr "Char code: [lindex $err 1]"
    puts stderr "Description: [lindex $err 2]"
  }
} else {
  puts "Response from instrument: $ans"
}

