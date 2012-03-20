# blue.tcl - Copyright (C) 2004 Pat Thoyts <patthoyts@users.sourceforge.net>
#
# blue.tcl,v 1.27 2005/10/08 14:56:57 jenglish Exp
#
#

namespace eval tile::theme::blue {

    package provide tile::theme::blue 0.7

    set imgdir [file join [file dirname [info script]] blue]
    array set I [tile::LoadImages $imgdir *.gif]

    array set colors {
	-frame  	"#6699cc"
	-lighter	"#bcd2e8"
	-window	 	"#e6f3ff"
	-selectbg	"#ffff33"
	-selectfg	"#000000"
	-disabledfg	"#666666"
    }

    style theme create blue -settings {

	style default . \
	    -borderwidth 	1 \
	    -background 	$colors(-frame) \
	    -fieldbackground	$colors(-window) \
	    -troughcolor	$colors(-lighter) \
	    -selectbackground	$colors(-selectbg) \
	    -selectforeground	$colors(-selectfg) \
	    ;
	style map . -foreground [list disabled $colors(-disabledfg)]

	## Buttons.
	#
	style default TButton -padding "10 0"
	style layout TButton {
	    Button.button -children {
		Button.focus -children {
		    Button.padding -children {
			Button.label
		    }
		}
	    }
	}

	style element create button image $I(button-n) \
	    -map [list pressed $I(button-p)  active $I(button-h)] \
	    -border 4 -sticky ew

	style element create Checkbutton.indicator image $I(check-nu) \
	    -width 24 -sticky w -map [list \
		{!disabled active selected} $I(check-hc) \
		{!disabled active} $I(check-hu) \
		{!disabled selected} $I(check-nc) ]

	style element create Radiobutton.indicator image $I(radio-nu) \
	    -width 24 -sticky w -map [list \
		{!disabled active selected} $I(radio-hc) \
		{!disabled active} $I(radio-hu) \
		selected $I(radio-nc) ]

	style default TMenubutton -relief raised -padding {10 2}

	## Toolbar buttons.
	#
	style default Toolbutton \
	    -width 0 -relief flat -borderwidth 2 -padding 4 \
	    -background $colors(-frame) -foreground #000000 ;
	style map Toolbutton -background [list active $colors(-selectbg)]
	style map Toolbutton -foreground [list active $colors(-selectfg)]
	style map Toolbutton -relief {
	    disabled 	flat
	    selected	sunken
	    pressed 	sunken
	    active  	raised
	}

	## Entry widgets.
	#
	style default TEntry \
	    -selectborderwidth 1 -padding 2 -insertwidth 2 -font TkTextFont
	style default TCombobox \
	    -selectborderwidth 1 -padding 2 -insertwidth 2 -font TkTextFont

	## Notebooks.
	#
	style default TNotebook.Tab -padding {4 2 4 2}
	style map TNotebook.Tab \
	    -background \
		[list selected $colors(-frame) active $colors(-lighter)] \
	    -padding [list selected {4 4 4 2}]

	## Labelframes.
	#
	style default TLabelframe -borderwidth 2 -relief groove

	## Scrollbars.
	#
	style layout Vertical.TScrollbar {
	    Scrollbar.trough -children {
		Scrollbar.uparrow -side top
		Scrollbar.downarrow -side bottom
		Scrollbar.uparrow -side bottom
		Vertical.Scrollbar.thumb -side top -expand true -sticky ns
	    }
	}

	style layout Horizontal.TScrollbar {
	    Scrollbar.trough -children {
		Scrollbar.leftarrow -side left
		Scrollbar.rightarrow -side right
		Scrollbar.leftarrow -side right
		Horizontal.Scrollbar.thumb -side left -expand true -sticky we
	    }
	}

	style element create Horizontal.Scrollbar.thumb image $I(sb-thumb) \
	    -map [list {pressed !disabled} $I(sb-thumb-p)] -border 3

	style element create Vertical.Scrollbar.thumb image $I(sb-vthumb) \
	    -map [list {pressed !disabled} $I(sb-vthumb-p)] -border 3

	foreach dir {up down left right} {
	    style element create ${dir}arrow image $I(arrow${dir}) \
		-map [list \
		    disabled $I(arrow${dir}) \
		    pressed $I(arrow${dir}-p) \
		    active $I(arrow${dir}-h)] \
	        -border 1 -sticky {}
	}

	## Scales.
	#
	style element create Scale.slider \
	    image $I(slider) -map [list {pressed !disabled} $I(slider-p)]

	style element create Vertical.Scale.slider \
	    image $I(vslider) -map [list {pressed !disabled} $I(vslider-p)]

	style element create Horizontal.Progress.bar \
	    image $I(sb-thumb) -border 2
	style element create Vertical.Progress.bar \
	    image $I(sb-vthumb) -border 2

    }
}

