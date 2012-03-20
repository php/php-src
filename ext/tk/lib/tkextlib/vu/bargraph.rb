#
#  ::vu::bargraph widget
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'

# create module/class
module Tk
  module Vu
    class Bargraph < TkWindow
    end
  end
end


# call setup script  --  <libdir>/tkextlib/vu.rb
require 'tkextlib/vu.rb'

class Tk::Vu::Bargraph < TkWindow
  TkCommandNames = ['::vu::bargraph'.freeze].freeze
  WidgetClassName = 'Bargraph'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  ###############################

  def __boolval_optkeys
    ['showminmax', 'showvalue']
  end
  private :__boolval_optkeys

  def __strval_optkeys
    super() + [
      'title',
      'barbackground', 'barcolor', 'barcolour',
      'tickcolor', 'tickcolour',
      'textcolor', 'textcolour',
    ]
  end
  private :__strval_optkeys

  def __listval_optkeys
    ['alabels', 'blabels']
  end
  private :__listval_optkeys

  def __font_optkeys
    ['alabfont', 'blabfont']
  end
  private :__font_optkeys

  ###############################

  def set(val = None)
    tk_call_without_enc(@path, 'set', val)
    self
  end

  def get()
    num_or_str(tk_call_without_enc(@path, 'get'))
  end
end
