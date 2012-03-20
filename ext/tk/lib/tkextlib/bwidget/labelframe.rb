#
#  tkextlib/bwidget/labelframe.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tk/frame'
require 'tkextlib/bwidget.rb'
require 'tkextlib/bwidget/label'

module Tk
  module BWidget
    class LabelFrame < TkWindow
    end
  end
end

class Tk::BWidget::LabelFrame
  TkCommandNames = ['LabelFrame'.freeze].freeze
  WidgetClassName = 'LabelFrame'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __strval_optkeys
    super() << 'helptext'
  end
  private :__strval_optkeys

  def __boolval_optkeys
    super() << 'dragenabled' << 'dropenabled'
  end
  private :__boolval_optkeys

  def __tkvariable_optkeys
    super() << 'helpvar'
  end
  private :__tkvariable_optkeys

  def self.align(*args)
    tk_call('LabelFrame::align', *args)
  end
  def get_frame(&b)
    win = window(tk_send_without_enc('getframe'))
    if b
      if TkCore::WITH_RUBY_VM  ### Ruby 1.9 !!!!
        win.instance_exec(self, &b)
      else
        win.instance_eval(&b)
      end
    end
    win
  end
end
