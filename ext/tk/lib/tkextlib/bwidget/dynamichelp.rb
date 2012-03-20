#
#  tkextlib/bwidget/dynamichelp.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/bwidget.rb'

module Tk
  module BWidget
    module DynamicHelp
    end
  end
end

module Tk::BWidget::DynamicHelp
  include Tk
  extend Tk

  def self.__pathname
    'DynamicHelp::configure'
  end

  def __strval_optkeys
    super() << 'topbackground'
  end
  private :__strval_optkeys

  def self.__cget_cmd
    ['DynamicHelp::configure']
  end

  def self.__config_cmd
    ['DynamicHelp::configure']
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

  def self.add(widget, keys={})
    tk_call('DynamicHelp::add', widget, *hash_kv(keys))
  end

  def self.delete(widget)
    tk_call('DynamicHelp::delete', widget)
  end

  def self.include(klass, type)
    tk_call('DynamicHelp::include', klass, type)
  end

  def self.sethelp(path, subpath, force=None)
    tk_call('DynamicHelp::sethelp', path, subpath, force)
  end
end
