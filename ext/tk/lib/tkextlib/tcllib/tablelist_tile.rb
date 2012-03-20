#
#  tkextlib/tcllib/tablelist_tlie.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * A multi-column listbox

require 'tk'
require 'tkextlib/tcllib.rb'

# TkPackage.require('tablelist_tile', '4.2')
TkPackage.require('tablelist_tile')

unless defined? Tk::Tcllib::Tablelist_usingTile
  Tk::Tcllib::Tablelist_usingTile = true
end

requrie 'tkextlib/tcllib/tablelist_core'

module Tk
  module Tcllib
    class Tablelist
      # commands related to tile Themems
      def self.set_theme(theme)
        Tk.tk_call('::tablelist::setTheme', theme)
      end

      def self.get_current_theme
        Tk.tk_call('::tablelist::getCurrentTheme')
      end

      def self.get_theme_list
        TkComm.simplelist(Tk.tk_call('::tablelist::getThemes'))
      end
      def self.set_theme_defaults
        Tk.tk_call('::tablelist::setThemeDefaults')
      end
    end

    Tablelist_Tile = Tablelist
    TableList_Tile = Tablelist
  end
end
