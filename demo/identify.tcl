#!/usr/bin/tclsh

##########################################################
#
# identify.tcl --
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
# This file  demonstrates basic input/output operations on  VISA instrument.
# We open an instrument, configure IO channel, send "clear" command and then
#   request instrument's ID string.
#
# Usage: identify.tcl [{VISA INSTR address}]
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

# get handle to default resource manager
if { [catch { set rm [visa::open-default-rm] } rc] } {
  puts stderr "Error opening default resource manager\n$rc"
}

# open device
if { [catch { set vi [visa::open $rm $visaAddr] } rc] } {
  puts "Error opening instrument `$visaAddr`\n$rc"

  # device opening error - exit
  # notice that we have no manually free `$rm` descriptor, 
  # Tcl does it itself!
  exit
}

# Send command to instrument. New line is added automatically by `puts`.
puts $vi "*CLS"

# Set zero timeout for non-blocking reading
# This is an equivalent of visa::set-attribute $vi $visa::ATTR_TMO_VALUE 0
fconfigure $vi -timeout 0

# Read all from input buffer.
while { [gets $vi ] != "" } {}

# Send command to query device identity string
puts $vi "*IDN?"

# This is an equivalent of visa::set-attribute $vi $visa::ATTR_TMO_VALUE 500
fconfigure $vi -timeout 500

# Read device's answer. Trailing new line char is removed by `gets`.
set id [gets $vi]

puts "Identity of `$visaAddr` is `$id`"

# close channels
close $vi
close $rm

