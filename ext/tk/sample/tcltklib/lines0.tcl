#! /usr/local/bin/wish

proc drawlines {} {
    puts [clock format [clock seconds]]

    for {set j 0} {$j < 100} {incr j} {
	puts -nonewline "*"
	flush stdout
	if {$j & 1} {
	    set c "blue"
	} {
	    set c "red"
	}
	for {set i 0} {$i < 100} {incr i} {
#	    .a create line $i 0 0 [expr 500 - $i] -fill $c
	}
    }

    puts [clock format [clock seconds]]

    for {set j 0} {$j < 100} {incr j} {
	puts -nonewline "*"
	flush stdout
	if {$j & 1} {
	    set c "blue"
	} {
	    set c "red"
	}
	for {set i 0} {$i < 100} {incr i} {
	    .a create line $i 0 0 [expr 500 - $i] -fill $c
	}
    }

    puts [clock format [clock seconds]]
#    destroy .
}

canvas .a -height 500 -width 500
button .b -text draw -command drawlines
pack .a .b -side left

# eof
