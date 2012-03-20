#
#  tkextlib/iwidgets/watch.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class Watch < Tk::Itk::Widget
    end
  end
end

class Tk::Iwidgets::Watch
  TkCommandNames = ['::iwidgets::watch'.freeze].freeze
  WidgetClassName = 'Watch'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __boolval_optkeys
    super() << 'showampm'
  end
  private :__boolval_optkeys

  def __strval_optkeys
    super() << 'clockcolor' << 'hourcolor' << 'minutecolor' <<
      'pivotcolor' << 'secondcolor' << 'tickcolor'
  end
  private :__strval_optkeys

  def get_string
    tk_call(@path, 'get', '-string')
  end
  alias get get_string

  def get_clicks
    number(tk_call(@path, 'get', '-clicks'))
  end

  def show(time=None)
    tk_call(@path, 'show', time)
    self
  end
  def show_now
    tk_call(@path, 'show', 'now')
    self
  end

  def watch(*args)
    unless args.empty?
      tk_call(@path, 'watch', *args)
    end
    component_path('canvas')
  end
end
