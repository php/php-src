#
# $Id$
#
# Demonstration of custom classes.
#
# The Tile button doesn't have built-in support for autorepeat.
# Instead of adding -repeatdelay and -repeatinterval options,
# and all the extra binding scripts required to deal with them,
# we create a custom widget class for autorepeating buttons.
#
# Usage:
#	ttk::button .b -class Repeater [... other options ...]
#
# TODO:
#	Use system settings for repeat interval and initial delay.
#
# Notes:
#	Repeater buttons work more like scrollbar arrows than
#	Tk repeating buttons: they fire once immediately when
#	first pressed, and $State(delay) specifies the initial
#	interval before the button starts autorepeating.
#

namespace eval tile::Repeater {
    variable State
    set State(timer) 	{}	;# [after] id of repeat script
    set State(interval)	100	;# interval between repetitions
    set State(delay)	300	;# delay after initial invocation
}

### Class bindings.
#

bind Repeater <Enter>		{ %W state active }
bind Repeater <Leave>		{ %W state !active }

bind Repeater <Key-space> 	{ tile::Repeater::Activate %W }
bind Repeater <<Invoke>> 	{ tile::Repeater::Activate %W }

bind Repeater <ButtonPress-1> 	{ tile::Repeater::Press %W }
bind Repeater <ButtonRelease-1> { tile::Repeater::Release %W }
bind Repeater <B1-Leave> 	{ tile::Repeater::Pause %W }
bind Repeater <B1-Enter> 	{ tile::Repeater::Resume %W } ;# @@@ see below

# @@@ Workaround for metacity-induced bug:
bind Repeater <B1-Enter> \
    { if {"%d" ne "NotifyUngrab"} { tile::Repeater::Resume %W } }

### Binding procedures.
#

## Activate -- Keyboard activation binding.
#	Simulate clicking the button, and invoke the command once.
#
proc tile::Repeater::Activate {w} {
    $w instate disabled { return }
    set oldState [$w state pressed]
    update idletasks; after 100
    $w state $oldState
    after idle [list $w invoke]
}

## Press -- ButtonPress-1 binding.
#	Invoke the command once and start autorepeating after
#	$State(delay) milliseconds.
#
proc tile::Repeater::Press {w} {
    variable State
    $w instate disabled { return }
    $w state pressed
    $w invoke
    after cancel $State(timer)
    set State(timer) [after $State(delay) [list tile::Repeater::Repeat $w]]
}

## Release -- ButtonRelease binding.
#	Stop repeating.
#
proc tile::Repeater::Release {w} {
    variable State
    $w state !pressed
    after cancel $State(timer)
}

## Pause -- B1-Leave binding
#	Temporarily suspend autorepeat.
#
proc tile::Repeater::Pause {w} {
    variable State
    $w state !pressed
    after cancel $State(timer)
}

## Resume -- B1-Enter binding
#	Resume autorepeat.
#
proc tile::Repeater::Resume {w} {
    variable State
    $w instate disabled { return }
    $w state pressed
    $w invoke
    after cancel $State(timer)
    set State(timer) [after $State(interval) [list tile::Repeater::Repeat $w]]
}

## Repeat -- Timer script
#	Invoke the command and reschedule another repetition
#	after $State(interval) milliseconds.
#
proc tile::Repeater::Repeat {w} {
    variable State
    $w instate disabled { return }
    $w invoke
    set State(timer) [after $State(interval) [list tile::Repeater::Repeat $w]]
}

#*EOF*
