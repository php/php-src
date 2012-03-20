#
#  tkextlib/iwidgets/datefield.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class  Datefield < Tk::Iwidgets::Labeledwidget
    end
  end
end

class Tk::Iwidgets::Datefield
  TkCommandNames = ['::iwidgets::datefield'.freeze].freeze
  WidgetClassName = 'Datefield'.freeze
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

  def show(date=None)
    tk_call(@path, 'show', date)
    self
  end
  def show_now
    tk_call(@path, 'show', 'now')
    self
  end
end
