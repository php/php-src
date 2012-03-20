#
#  tkextlib/bwidget/widget.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/bwidget.rb'

module Tk
  module BWidget
    module Widget
    end
  end
end

module Tk::BWidget::Widget
  include Tk
  extend Tk

  def self.__pathname
    'Widget::configure'
  end

  def self.__cget_cmd
    ['Widget::cget']
  end

  def self.__config_cmd
    ['Widget::configure']
  end

  def self.cget_strict(slot)
    slot = slot.to_s
    info = {}
    self.current_configinfo.each{|k,v| info[k.to_s] = v if k.to_s == slot}
    fail RuntimeError, "unknown option \"-#{slot}\""  if info.empty?
    info.values[0]
  end
  def self.cget(slot)
    self.current_configinfo(slot).values[0]
  end

  def self.add_map(klass, subclass, subpath, opts)
    tk_call('Widget::addmap', klass, subclass, subpath, opts)
  end

  def self.bwinclude(klass, subclass, subpath, *args)
    tk_call('Widget::bwinclude', klass, subclass, subpath, *args)
  end

  def self.create(klass, path, rename=None, &b)
    win = window(tk_call('Widget::create', klass, path, rename))
    if b
      if TkCore::WITH_RUBY_VM  ### Ruby 1.9 !!!!
        win.instance_exec(self, &b)
      else
        win.instance_eval(&b)
      end
    end
    win
  end

  def self.declare(klass, optlist)
    tk_call('Widget::declare', klass, optlist)
  end

  def self.define(klass, filename, *args)
    tk_call('Widget::define', klass, filename, *args)
  end

  def self.destroy(win)
    tk_call('Widget::destroy', _epath(win))
  end

  def self.focus_next(win)
    tk_call('Widget::focusNext', win)
  end

  def self.focus_ok(win)
    tk_call('Widget::focusOk', win)
  end

  def self.focus_prev(win)
    tk_call('Widget::focusPrev', win)
  end

  def self.generate_doc(dir, widgetlist)
    tk_call('Widget::generate-doc', dir, widgetlist)
  end

  def self.generate_widget_doc(klass, iscmd, file)
    tk_call('Widget::generate-widget-doc', klass, iscmd, file)
  end

  def self.get_option(win, option)
    tk_call('Widget::getoption', win, option)
  end

  def self.get_variable(win, varname, my_varname=None)
    tk_call('Widget::getVariable', win, varname, my_varname)
  end

  def self.has_changed(win, option, pvalue)
    tk_call('Widget::hasChanged', win, option, pvalue)
  end

  def self.init(klass, win, options)
    tk_call('Widget::init', klass, win, options)
  end

  def self.set_option(win, option, value)
    tk_call('Widget::setoption', win, option, value)
  end

  def self.sub_cget_strict(win, subwidget)
    tk_call('Widget::subcget', win, subwidget)
  end
  def self.sub_cget(win, subwidget)
    self.sub_cget_strict(win, subwidget)
  end

  def self.sync_options(klass, subclass, subpath, options)
    tk_call('Widget::syncoptions', klass, subclass, subpath, options)
  end

  def self.tkinclude(klass, tkwidget, subpath, *args)
    tk_call('Widget::tkinclude', klass, tkwidget, subpath, *args)
  end
end
