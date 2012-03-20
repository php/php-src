# kroc.tcl - Copyright (C) 2004 David Zolli <kroc@kroc.tk>
#
# A sample pixmap theme for the tile package.

#package require tile::pixmap

namespace eval tile {
    namespace eval kroc {
        variable version 0.0.1
    }
}

namespace eval tile::kroc {

    set imgdir [file join [file dirname [info script]] kroc]
    array set Images [tile::LoadImages $imgdir *.gif]

    if {[package vsatisfies [package provide tile] 0.5]} {
        set TNoteBook_Tab TNotebook.Tab
    } else {
        set TNoteBook_Tab Tab.TNotebook
    }

    style theme create kroc -parent alt -settings {

        style default . -background #FCB64F -troughcolor #F8C278 -borderwidth 1
	style default . -font TkDefaultFont -borderwidth 1
        style map . -background [list active #694418]
        style map . -foreground [list disabled #B2B2B2 active #FFE7CB]

        style default TButton -padding "10 4"

        style default $TNoteBook_Tab -padding {10 3} -font TkDefaultFont
        style map $TNoteBook_Tab \
                -background [list selected #FCB64F {} #FFE6BA] \
                -foreground [list {} black] \
                -padding [list selected {10 6 10 3}]

        style map TScrollbar \
		-background	{ pressed #694418} \
                -arrowcolor	{ pressed #FFE7CB } \
                -relief		{ pressed sunken } \
                ;

        style layout Vertical.TScrollbar {
            Scrollbar.trough -children {
                Scrollbar.uparrow -side top
                Scrollbar.downarrow -side bottom
                Scrollbar.uparrow -side bottom
                Scrollbar.thumb -side top -expand true
            }
        }

        style layout Horizontal.TScrollbar {
            Scrollbar.trough -children {
                Scrollbar.leftarrow -side left
                Scrollbar.rightarrow -side right
                Scrollbar.leftarrow -side right
                Scrollbar.thumb -side left -expand true
            }
        }

        #
        # Elements:
        #
        if {[package vsatisfies [package provide tile] 0.5]} {

            style element create Button.button image $Images(button-n) \
                -map [list  \
                    pressed		$Images(button-p) \
                    active		$Images(button-h) \
                    ] -border 3 -sticky ew

            style element create Checkbutton.indicator image $Images(check-nu) \
                -map [list \
                    {pressed selected}	$Images(check-nc) \
                    pressed		$Images(check-nu) \
                    {active selected}	$Images(check-hc) \
                    active		$Images(check-hu) \
                    selected		$Images(check-nc) \
                    ] -sticky w

            style element create Radiobutton.indicator image $Images(radio-nu) \
                -map [list \
                    {pressed selected}	$Images(radio-nc) \
                    pressed		$Images(radio-nu) \
                    {active selected}	$Images(radio-hc) \
                    active		$Images(radio-hu) \
                    selected		$Images(radio-nc) \
                    ] -sticky w

        } else {

            style element create Button.button pixmap -images [list  \
                    pressed		$Images(button-p) \
                    active		$Images(button-h) \
                    {}			$Images(button-n) \
                    ] -border 3 -tiling tile

            style element create Checkbutton.indicator pixmap -images [list \
                    {pressed selected}	$Images(check-nc) \
                    pressed		$Images(check-nu) \
                    {active selected}	$Images(check-hc) \
                    active		$Images(check-hu) \
                    selected		$Images(check-nc) \
                    {}			$Images(check-nu) \
                    ] -tiling fixed

            style element create Radiobutton.indicator pixmap -images [list \
                    {pressed selected}	$Images(radio-nc) \
                    pressed		$Images(radio-nu) \
                    {active selected}	$Images(radio-hc) \
                    active		$Images(radio-hu) \
                    selected		$Images(radio-nc) \
                    {}			$Images(radio-nu) \
                    ] -tiling fixed

        }

        #
        # Settings: (*button.background is not needed in tile 0.5 or above)
        #
        style layout TButton {
	    Button.button -children {
		Button.focus -children {
		    Button.padding -children {
			Button.label -expand true -sticky {}
		    }
		}
	    }
        }

        style layout TCheckbutton {
	    Checkbutton.border -children {
		Checkbutton.background
		Checkbutton.padding -children {
		    Checkbutton.indicator -side left
		    Checkbutton.focus -side left -children {
			Checkbutton.label
		    }
		}
            }
        }

        style layout TRadiobutton {
            Radiobutton.border -children {
                Radiobutton.background
                Radiobutton.padding -children  {
                    Radiobutton.indicator -side left
                    Radiobutton.focus -expand true -sticky w -children {
                        Radiobutton.label -side right -expand true
                    }
                }
            }
        }

    } }

# -------------------------------------------------------------------------

package provide tile::theme::kroc $::tile::kroc::version

# -------------------------------------------------------------------------
