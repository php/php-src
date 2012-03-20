#
# $Id$
#
# Demonstration of custom widget styles.
#

#
# ~ BACKGROUND
#
# Checkbuttons in toolbars have a very different appearance
# than regular checkbuttons: there's no indicator, they
# "pop up" when the mouse is over them, and they appear sunken
# when selected.
#
# Tk added partial support for toolbar-style buttons in 8.4
# with the "-overrelief" option, and TIP #82 added further
# support with the "-offrelief" option.  So to get a toolbar-style
# checkbutton, you can configure it with:
#
# checkbutton .cb \
#     -indicatoron false -selectcolor {} -relief flat -overrelief raised
#
# Behind the scenes, Tk has a lot of rather complicated logic
# to implement this checkbutton style; see library/button.tcl,
# generic/tkButton.c, and the platform-specific files unix/tkUnixButton.c
# et al. for the full details.
#
# The tile widget set has a better way: custom styles.
# Since the appearance is completely controlled by the theme engine,
# we can define a new "Toolbutton" style and just use:
#
# checkbutton .cb -style Toolbutton
#
#
# ~ DEMONSTRATION
#
# The tile built-in themes (default, "alt", windows, and XP)
# already include Toolbutton styles.  This script will add
# them to the "step" and "blue" themes as a demonstration.
#
# (Note: Pushbuttons and radiobuttons can also use the "Toolbutton"
# style; see demo.tcl.)
#

style theme settings "step" {

#
# First, we use [style layout] to define what elements to
# use and how they're arranged.  Toolbuttons are pretty
# simple, consisting of a border, some internal padding,
# and a label.  (See also the TScrollbar layout definition
# in demos/blue.tcl for a more complicated layout spec.)
#
    style layout Toolbutton {
        Toolbutton.background
        Toolbutton.border -children {
            Toolbutton.padding -children {
                Toolbutton.label
            }
        }
    }

# (Actually the above isn't strictly necessary, since the same layout
# is defined in the default theme; we could have inherited it
# instead.)
#
# Next, specify default values for element options.
# For many options (like -background), the defaults
# inherited from the parent style are sufficient.
#
    style default Toolbutton -width 0 -padding 1 -relief flat -borderwidth 2

#
# Finally, use [style map] to specify state-specific
# resource values.  We want a flat relief if the widget is
# disabled, sunken if it's selected (on) or pressed,
# and raised when it's active (the mouse pointer is
# over the widget).  Each state-value pair is checked
# in order, and the first matching state takes precedence.
#
    style map Toolbutton -relief {
	disabled 	flat
	selected	sunken
	pressed 	sunken
	active		raised
    }
}

#
# Now for the "blue" theme.  (Since the purpose of this
# theme is to show what *can* be done, not necessarily what
# *should* be done, the following makes some questionable
# design decisions from an aesthetic standpoint.)
#
if {![catch {package require tile::theme::blue}]} {
style theme settings "blue" {

    #
    # Default values:
    #
    style default Toolbutton \
    	-width 0 -relief flat -borderwidth 2 \
	-background #6699CC -foreground #000000 ;

    #
    # Configure state-specific values for -relief, as before:
    #
    style map Toolbutton -relief {
	disabled 	flat
	selected	sunken
	pressed 	sunken
	active		raised
    }

    #
    # Adjust the -padding at the same time, to enhance
    # the raised/sunken illusion:
    #
    style default Toolbutton -padding 4
    style map Toolbutton -padding {
	disabled	{4}
	selected	{6 6 2 2}
	pressed		{6 6 2 2}
	active		{2 2 6 6}
    }

    #
    # ... and change the foreground and background colors
    # when the mouse cursor is over the widget:
    #
    style map Toolbutton -background {
	active  	#008800
    } -foreground {
	active  	#FFFFFF
    }
}

}

#
# ~ A final note:
#
# TIP #82 also says: "When -indicatoron is off and the button itself
# is on, the relief continues to be hard-coded to sunken. For symmetry,
# we might consider adding another -onrelief option to cover this
# case. But it is difficult to imagine ever wanting to change the
# value of -onrelief so it has been omitted from this TIP.
# If there as strong desire to have -onrelief, it can be added later."
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#
# The Tile project aims to make sure that this never needs to happen.
#
