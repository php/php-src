#
#  tkextlib/iwidgets/timefield.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class  Timefield < Tk::Iwidgets::Labeledwidget
    end
  end
end

class Tk::Iwidgets::Timefield
  TkCommandNames = ['::iwidgets::timefield'.freeze].freeze
  WidgetClassName = 'Timefield'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __boolval_optkeys
    super() << 'gmt'
  end
  private :__boolval_optkeys

  def __strval_optkeys
    super() << 'textbackground'
  end
  private :__strval_optkeys

  def __font_optkeys
    super() << 'textfont'
  end
  private :__font_optkeys

  def get_string
    tk_call(@path, 'get', '-string')
  end
  alias get get_string

  def get_clicks
    number(tk_call(@path, 'get', '-clicks'))
  end

  def valid?
    bool(tk_call(@path, 'isvalid'))
  end
  alias isvalid? valid?

  def show(time=None)
    tk_call(@path, 'show', time)
    self
  end
  def show_now
    tk_call(@path, 'show', 'now')
    self
  end
end
