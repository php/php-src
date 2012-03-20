# plastik.tcl - Copyright (C) 2004 Googie
#
# A sample pixmap theme for the tile package.
#
#  Copyright (c) 2004 Googie
#  Copyright (c) 2005 Pat Thoyts <patthoyts@users.sourceforge.net>
#
# $Id$

package require Tk 8.4
package require tile 0.5

namespace eval tile::theme::plastik {

    variable version 0.3.1
    package provide tile::theme::plastik $version

    variable imgdir [file join [file dirname [info script]] plastik]
    variable Images;
    array set Images [tile::LoadImages $imgdir *.gif]

    variable colors
    array set colors {
    	-frame 		"#cccccc"
	-disabledfg	"#aaaaaa"
	-selectbg	"#657a9e"
	-selectfg	"#ffffff"
    }

style theme create plastik -parent default -settings {
    style default . \
    	-background $colors(-frame) \
	-troughcolor $colors(-frame) \
	-selectbackground $colors(-selectbg) \
	-selectforeground $colors(-selectfg) \
	-font TkDefaultFont \
	-borderwidth 1 \
	;

    style map . -foreground [list disabled $colors(-disabledfg)]

    #
    # Layouts:
    #
    style layout Vertical.TScrollbar {
        Scrollbar.background
        Scrollbar.trough -children {
            Scrollbar.uparrow -side top
            Scrollbar.downarrow -side bottom
            Scrollbar.uparrow -side bottom
            Vertical.Scrollbar.thumb -side top -expand true -sticky ns
        }
    }

    style layout Horizontal.TScrollbar {
        Scrollbar.background
        Scrollbar.trough -children {
            Scrollbar.leftarrow -side left
            Scrollbar.rightarrow -side right
            Scrollbar.leftarrow -side right
            Horizontal.Scrollbar.thumb -side left -expand true -sticky we
        }
    }

    style layout TButton {
        Button.button -children {
	    Button.focus -children {
		Button.padding -children {
		    Button.label -side left -expand true
		}
	    }
	}
    }

    #
    # Elements:
    #
    style element create Button.button image $Images(button-n) \
    	-border 4 -sticky ew \
    	-map [list pressed $Images(button-p)  active $Images(button-h)]

    style element create Checkbutton.indicator image $Images(check-nu) \
	-sticky {} -map [list \
	    {active selected}     $Images(check-hc) \
	    {pressed selected}    $Images(check-pc) \
	    active                $Images(check-hu) \
	    selected              $Images(check-nc) \
	]

    style element create Radiobutton.indicator image $Images(radio-nu) \
	-sticky {} -map [list \
	    {active selected}     $Images(radio-hc) \
	    {pressed selected}    $Images(radio-pc) \
	    active                $Images(radio-hu) \
	    selected              $Images(radio-nc) \
	]

    style element create Horizontal.Scrollbar.thumb \
    	image $Images(hsb-n) -border 3 -sticky ew
    style element create Vertical.Scrollbar.thumb \
    	image $Images(vsb-n) -border 3 -sticky ns

    style element create Scale.slider \
    	image $Images(hslider-n) -sticky {}
    style element create Vertical.Scale.slider \
    	image $Images(vslider-n) -sticky {}

    style element create Scrollbar.uparrow image $Images(arrowup-n) \
    	-map [list pressed $Images(arrowup-p)] -sticky {}
    style element create Scrollbar.downarrow image $Images(arrowdown-n) \
    	-map [list pressed $Images(arrowdown-p)] -sticky {}
    style element create Scrollbar.leftarrow image $Images(arrowleft-n) \
    	-map [list pressed $Images(arrowleft-p)] -sticky {}
    style element create Scrollbar.rightarrow image $Images(arrowright-n) \
    	-map [list pressed $Images(arrowright-p)] -sticky {}

    #
    # Settings:
    #
    style default TButton -width -10
    style default TNotebook.Tab -padding {6 2 6 2}
    style default TLabelframe -borderwidth 2 -relief groove

} }

