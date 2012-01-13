#!/usr/bin/tclsh

##########################################################
#
# fconfigure.tcl --
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
# This file  demonstrates usage of `fconfigure` Tcl command
#   on VISA devices
#
# Usage: fconfigure.tcl [{VISA address}]
#
##########################################################

package require tclvisa

set DEFAULT_ADDR "ASRL1::INSTR"

##########################################################
# procedure sets a new value to channel's option
# then it reads the option from channe and compares
##########################################################
proc checkOption { vi option value } {
  fconfigure $vi $option $value
  set v [fconfigure $vi $option]
  set mismatch ""
  if { $value != $v } {
    set mismatch "MISMATCH!"
  }
  puts "set `$value`, get `$v` $mismatch"
}

##########################################################
# procedure prints xon/xoff chars
##########################################################
proc printXchars { xchar } {
  binary scan [lindex $xchar 0] c xon
  binary scan [lindex $xchar 1] c xoff
  return "xon=[format %02xh $xon], xoff=[format %02xh $xoff]"
}

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

# check interface type of the instrument opened
# serial options only allowed for "serial" interface type
if { [visa::get-attribute $vi $visa::ATTR_INTF_TYPE] != $visa::INTF_ASRL } {
	puts "Instrument has non-serial interface type"
	exit
}

##########################################################
# get/set serial mode
##########################################################

puts "Initial mode=[set mode [fconfigure $vi -mode]]"

# set different parity modes
checkOption $vi -mode 9600,o,8,1
checkOption $vi -mode 9600,e,8,1
checkOption $vi -mode 9600,m,8,1
checkOption $vi -mode 9600,s,8,1
checkOption $vi -mode 9600,n,8,1

# set different baud rates
checkOption $vi -mode 300,n,8,1
checkOption $vi -mode 600,n,8,1
checkOption $vi -mode 1200,n,8,1
checkOption $vi -mode 2400,n,8,1
checkOption $vi -mode 4800,n,8,1
checkOption $vi -mode 9600,n,8,1
checkOption $vi -mode 19200,n,8,1
checkOption $vi -mode 38400,n,8,1
checkOption $vi -mode 57600,n,8,1
checkOption $vi -mode 115200,n,8,1

# set diffent numbers of data bits
checkOption $vi -mode 9600,n,5,1
checkOption $vi -mode 9600,n,6,1
checkOption $vi -mode 9600,n,7,1

# set diffent numbers of stop bits
checkOption $vi -mode 9600,n,8,1
checkOption $vi -mode 9600,n,8,2

# restore initial mode
fconfigure $vi -mode $mode

##########################################################
# get/set timeout
##########################################################

puts "Initial timeout=[set timeout [fconfigure $vi -timeout]]"

checkOption $vi -timeout 100
checkOption $vi -timeout 1000
checkOption $vi -timeout 10000

# notice usage of predefined VISA constants
checkOption $vi -timeout $visa::TMO_IMMEDIATE
checkOption $vi -timeout $visa::TMO_INFINITE

# restore initial timeout
fconfigure $vi -timeout $timeout

##########################################################
# get/set flow control
##########################################################

puts "Initial handshake=[set handshake [fconfigure $vi -handshake]]"

checkOption $vi -handshake xonxoff
checkOption $vi -handshake rtscts
checkOption $vi -handshake dtrdsr
checkOption $vi -handshake none

# restore initial handshake 
fconfigure $vi -handshake $handshake

##########################################################
# get/set xon/xoff chars
##########################################################

set xchar [fconfigure $vi -xchar]
binary scan [lindex $xchar 0] c xon
binary scan [lindex $xchar 1] c xoff
puts "Initial [printXchars $xchar]"

fconfigure $vi -xchar {"\x03" "\x04"}
puts "New [printXchars [fconfigure $vi -xchar]]"

# restore initial handshake 
fconfigure $vi -xchar $xchar

##########################################################
# set modem status
##########################################################

fconfigure $vi -ttycontrol { DTR 0 RTS 0 }
fconfigure $vi -ttycontrol { DTR 0 RTS 1 }
fconfigure $vi -ttycontrol { DTR 1 RTS 0 }
fconfigure $vi -ttycontrol { DTR 1 RTS 1 }

##########################################################
# get modem status
##########################################################

puts "Modem bits=[fconfigure $vi -ttystatus]"

##########################################################
# get number of bytes in in/out buffers
##########################################################

puts "Bytes in input/output buffers=[fconfigure $vi -queue]"

##########################################################
# SCRIPT END
##########################################################

# all handles we opened can be closed automatically by Tcl
# but we also can close then manually

close $vi
close $rm
