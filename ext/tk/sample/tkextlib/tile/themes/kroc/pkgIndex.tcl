# pkgIndex.tcl for additional tile pixmap themes.
#
# We don't provide the package is the image subdirectory isn't present,
# or we don't have the right version of Tcl/Tk
#
# To use this automatically within tile, the tile-using application should
# use tile::availableThemes and tile::setTheme
#
# $Id$

if {![file isdirectory [file join $dir kroc]]} { return }
if {![package vsatisfies [package provide Tcl] 8.4]} { return }

package ifneeded tile::theme::kroc 0.0.1 \
    [list source [file join $dir kroc.tcl]]
