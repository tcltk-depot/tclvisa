#!/usr/bin/tclsh

##########################################################
#
# exclusive-lock.tcl --
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
# This file  demonstrates usage of `visa::lock` and `visa::unlock` commands,
#   which call viLock and viUnlock VISA API functions correspondingly.
# In this demo we open two sessions to the same instrument in two parallel 
#   threads.
# Then we try to get a lock to the instrument from both threads.
#
# Usage: exclusive-lock.tcl [{VISA INSTR address}]
#
##########################################################

package require tclvisa
package require Thread

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

# assign execution script to a variable
set script {
	package require tclvisa

	# main thread procedure
	proc run { visaAddr threadNum } {
		# get handle to default resource manager
		# in Tcl channels are not shared between threads
		# so we have to open one resource manager session per thread
		if { [catch { set rm [visa::open-default-rm] } rc] } {
			puts stderr "Error opening default resource manager\n$rc"
			thread::exit
		}

		# open device
		puts "Thread #${threadNum}: Opening device `$visaAddr`"
		if { [catch { set vi [visa::open $rm $visaAddr] } rc] } {
		  puts "Error opening instrument `$visaAddr`\n$rc"
		  thread::exit
		}
		puts "Thread #${threadNum}: Device is opened"

		puts "Thread #${threadNum}: Getting lock"
		# will wait forever because timeout is not specified
		visa::lock $vi
		puts "Thread1 #${threadNum}: Lock is asquired"

		# we have exclusive access to the instrument now
		# imitate some actions on instrument
		after 3000

		puts "Thread #${threadNum}: Releasing lock"
		visa::unlock $vi
		puts "Thread1 #${threadNum}: Lock is released"

		close $vi
		close $rm
	}
	
	# enter to event loop
	thread::wait
}

# create threads
puts "Main thread: create child threads"
set t1 [thread::create -joinable $script]
set t2 [thread::create -joinable $script]

# start processing in threads
puts "Main thread: run child threads"
thread::send -async $t1 "run $visaAddr 1"
thread::send -async $t2 "run $visaAddr 2"

thread::send -async $t1 "thread::exit"
thread::send -async $t2 "thread::exit"

# wait for completion
puts "Main thread: wait for child threads"
thread::join $t1
thread::join $t2

puts "Main thread: end"

