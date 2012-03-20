#
#  tkextlib/bwidget/mainframe.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tk/frame'
require 'tkextlib/bwidget.rb'
require 'tkextlib/bwidget/progressbar'

module Tk
  module BWidget
    class MainFrame < TkWindow
    end
  end
end

class Tk::BWidget::MainFrame
  TkCommandNames = ['MainFrame'.freeze].freeze
  WidgetClassName = 'MainFrame'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __strval_optkeys
    super() << 'progressfg'
  end
  private :__strval_optkeys

  def __tkvariable_optkeys
    super() << 'progressvar'
  end
  private :__tkvariable_optkeys

  def __val2ruby_optkeys  # { key=>proc, ... }
    # The method is used to convert a opt-value to a ruby's object.
    # When get the value of the option "key", "proc.call(value)" is called.
    {
      'menu'=>proc{|v| simplelist(v).collect!{|elem| simplelist(v)}}
    }
  end
  private :__val2ruby_optkeys

  def add_indicator(keys={}, &b)
    win = window(tk_send('addindicator', *hash_kv(keys)))
    if b
      if TkCore::WITH_RUBY_VM  ### Ruby 1.9 !!!!
        win.instance_exec(self, &b)
      else
        win.instance_eval(&b)
      end
    end
    win
  end

  def add_toolbar(&b)
    win = window(tk_send('addtoolbar'))
    if b
      if TkCore::WITH_RUBY_VM  ### Ruby 1.9 !!!!
        win.instance_exec(self, &b)
      else
        win.instance_eval(&b)
      end
    end
    win
  end

  def get_frame(&b)
    win = window(tk_send('getframe'))
    if b
      if TkCore::WITH_RUBY_VM  ### Ruby 1.9 !!!!
        win.instance_exec(self, &b)
      else
        win.instance_eval(&b)
      end
    end
    win
  end

  def get_indicator(idx, &b)
    win = window(tk_send('getindicator', idx))
    if b
      if TkCore::WITH_RUBY_VM  ### Ruby 1.9 !!!!
        win.instance_exec(self, &b)
      else
        win.instance_eval(&b)
      end
    end
    win
  end

  def get_menu(menu_id, &b)
    win = window(tk_send('getmenu', menu_id))
    if b
      if TkCore::WITH_RUBY_VM  ### Ruby 1.9 !!!!
        win.instance_exec(self, &b)
      else
        win.instance_eval(&b)
      end
    end
    win
  end

  def get_toolbar(idx, &b)
    win = window(tk_send('gettoolbar', idx))
    if b
      if TkCore::WITH_RUBY_VM  ### Ruby 1.9 !!!!
        win.instance_exec(self, &b)
      else
        win.instance_eval(&b)
      end
    end
    win
  end

  def get_menustate(tag)
    tk_send('getmenustate', tag) # return state name string
  end

  def set_menustate(tag, state)
    tk_send('setmenustate', tag, state)
    self
  end

  def show_statusbar(name)
    tk_send('showstatusbar', name)
    self
  end

  def show_toolbar(idx, mode)
    tk_send('showtoolbar', idx, mode)
    self
  end
end
