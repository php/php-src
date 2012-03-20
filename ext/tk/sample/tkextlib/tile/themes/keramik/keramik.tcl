# keramik.tcl -
#
# A sample pixmap theme for the tile package.
#
#  Copyright (c) 2004 Googie
#  Copyright (c) 2004 Pat Thoyts <patthoyts@users.sourceforge.net>
#
# $Id$

package require Tk 8.4;                 # minimum version for Tile
package require tile 0.5;               # depends upon tile 0.5

namespace eval tile {
    namespace eval theme {
        namespace eval keramik {
            variable version 0.3.2
        }
    }
}

namespace eval tile::theme::keramik {

    variable imgdir [file join [file dirname [info script]] keramik]
    variable I
    array set I [tile::LoadImages $imgdir *.gif]

    variable colors
    array set colors {
        -frame      "#cccccc"
        -lighter    "#cccccc"
        -window     "#ffffff"
        -selectbg   "#eeeeee"
        -selectfg   "#000000"
        -disabledfg "#aaaaaa"
    }

    style theme create keramik -parent alt -settings {


        # -----------------------------------------------------------------
        # Theme defaults
        #
        style default . \
            -borderwidth 1 \
            -background $colors(-frame) \
            -troughcolor $colors(-lighter) \
            -font TkDefaultFont \
            ;

        style map . -foreground [list disabled $colors(-disabledfg)]

        # -----------------------------------------------------------------
        # Button elements
        #  - the button has a large rounded border and needs a bit of
        #    horizontal padding.
        #  - the checkbutton and radiobutton have the focus drawn around
        #    the whole widget - hence the new layouts.
        #
        style layout TButton {
            Button.background
            Button.button -children {
                Button.focus -children {
                    Button.label
                }
            }
        }
        style layout Toolbutton {
            Toolbutton.background
            Toolbutton.button -children {
                Toolbutton.focus -children {
                    Toolbutton.label
                }
            }
        }
        style element create button image $I(button-n) \
            -border {8 6 8 16} -padding {6 6} -sticky news \
            -map [list {pressed !disabled} $I(button-p) \
                      {active !selected}  $I(button-h) \
                      selected $I(button-s) \
                      disabled $I(button-d)]
        style default TButton -padding {10 6}

        style element create Toolbutton.button image $I(tbar-n) \
            -border {2 8 2 16} -padding {2 2} -sticky news \
            -map [list {pressed !disabled} $I(tbar-p) \
                      {active !selected}   $I(tbar-a) \
                      selected             $I(tbar-p)]

        style element create Checkbutton.indicator image $I(check-u) \
            -width 20 -sticky w \
            -map [list selected $I(check-c)]

        style element create Radiobutton.indicator image $I(radio-u) \
            -width 20 -sticky w \
            -map [list  selected $I(radio-c)]

        # The layout for the menubutton is modified to have a button element
        # drawn on top of the background. This means we can have transparent
        # pixels in the button element. Also, the pixmap has a special
        # region on the right for the arrow. So we draw the indicator as a
        # sibling element to the button, and draw it after (ie on top of) the
        # button image.
        style layout TMenubutton {
            Menubutton.background
            Menubutton.button -children {
                Menubutton.focus -children {
                    Menubutton.padding -children {
                        Menubutton.label -side left -expand true
                    }
                }
            }
            Menubutton.indicator -side right
        }
        style element create Menubutton.button image $I(mbut-n) \
            -map [list {active !disabled} $I(mbut-a) \
                      {pressed !disabled} $I(mbut-a) \
                      {disabled}          $I(mbut-d)] \
            -border {7 10 29 15} -padding {7 4 29 4} -sticky news
        style element create Menubutton.indicator image $I(mbut-arrow-n) \
            -width 11 -sticky w -padding {0 0 18 0}

        # -----------------------------------------------------------------
        # Scrollbars, scale and progress elements
        #  - the scrollbar has three arrow buttons, two at the bottom and
        #    one at the top.
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

        style default TScrollbar -width 16

        style element create Horizontal.Scrollbar.thumb image $I(hsb-n) \
            -border {6 4} -width 15 -height 16 -sticky news \
            -map [list {pressed !disabled} $I(hsb-p)]

        style element create Vertical.Scrollbar.thumb image $I(vsb-n) \
            -border {4 6} -width 16 -height 15 -sticky news \
            -map [list {pressed !disabled} $I(vsb-p)]

        style element create Scale.slider image $I(hslider-n) \
            -border 3

        style element create Vertical.Scale.slider image $I(vslider-n) \
            -border 3

        style element create Horizontal.Progress.bar image $I(hsb-n) \
            -border {6 4}

        style element create Vertical.Progress.bar image $I(vsb-n) \
            -border {4 6}

        style element create uparrow image $I(arrowup-n) \
            -map [list {pressed !disabled} $I(arrowup-p)]

        style element create downarrow image $I(arrowdown-n) \
            -map [list {pressed !disabled} $I(arrowdown-p)]

        style element create rightarrow image $I(arrowright-n) \
            -map [list {pressed !disabled} $I(arrowright-p)]

        style element create leftarrow image $I(arrowleft-n) \
            -map [list {pressed !disabled} $I(arrowleft-p)]

        # -----------------------------------------------------------------
        # Notebook elements
        #
        style element create tab image $I(tab-n) \
            -map [list selected $I(tab-p) active $I(tab-p)] \
            -border {6 6 6 2} -height 12

	## Labelframes.
	#
	style default TLabelframe -borderwidth 2 -relief groove
    }
}

package provide tile::theme::keramik $::tile::theme::keramik::version
