# -*- tcl -*-
# Tcl package index file, version 1.1
#
if {[package vsatisfies [package provide Tcl] 8.4]} {
    package ifneeded tclvisa 0.3.0 [list load [file join $dir tclvisa030.dll]]
}
