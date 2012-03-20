#
#   tk/palette.rb : methods for Tcl/Tk standard library 'palette.tcl'
#                     1998/06/21 by Hidetoshi Nagai <nagai@ai.kyutech.ac.jp>
#
require 'tk'

module TkPalette
  include Tk
  extend Tk

  TkCommandNames = [
    'tk_setPalette'.freeze,
    'tk_bisque'.freeze,
    'tkDarken'.freeze
  ].freeze

  def TkPalette.set(*args)
    args = args[0].to_a.flatten if args[0].kind_of? Hash
    tk_call('tk_setPalette', *args)
  end
  def TkPalette.setPalette(*args)
    TkPalette.set(*args)
  end

  def TkPalette.bisque
    tk_call('tk_bisque')
  end

  def TkPalette.darken(color, percent)
    tk_call('tkDarken', color, percent)
  end

  def TkPalette.recolorTree(win, colors)
    if not colors.kind_of?(Hash)
      fail "2nd arg need to be Hash"
    end

    tk_call('global', "tkPalette")
    colors.each{|key, value|
      begin
        if win.cget(key) == tk_call('set', "tkPalette(#{key})")
          win[key] = colors[key]
        end
      rescue
        # ignore
      end
    }

    TkWinfo.children(win).each{|w| TkPalette.recolorTree(w, colors)}
  end

  def recolorTree(colors)
    TkPalette.recolorTree(self, colors)
  end
end
