#
#  tkextlib/tcllib/tooltip.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * Provides tooltips, a small text message that is displayed when the
#     mouse hovers over a widget.
#

require 'tk'
require 'tkextlib/tcllib.rb'

# TkPackage.require('tooltip', '1.1')
TkPackage.require('tooltip')

module Tk::Tcllib
  module Tooltip
    PACKAGE_NAME = 'tooltip'.freeze
    def self.package_name
      PACKAGE_NAME
    end

    def self.package_version
      begin
        TkPackage.require('tooltip')
      rescue
        ''
      end
    end
  end
end

module Tk::Tcllib::Tooltip
  extend TkCore

  WidgetClassName = 'Tooltip'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def self.database_classname
    self::WidgetClassName
  end
  def self.database_class
    WidgetClassNames[self::WidgetClassName]
  end

  def self.clear(glob_path_pat = None)
    self.clear_glob(glob_path_pat)
  end

  def self.clear_glob(glob_path_pat)
    tk_call_without_enc('::tooltip::tooltip', 'clear', glob_path_pat)
  end

  def self.clear_widgets(*args)
    self.clear_glob("{#{args.collect{|w| _get_eval_string(w)}.join(',')}}")
  end

  def self.clear_children(*args)
    self.clear_glob("{#{args.collect{|w| s = _get_eval_string(w); "#{s},#{s}.*"}.join(',')}}")
  end

  def self.delay(millisecs=None)
    number(tk_call_without_enc('::tooltip::tooltip', 'delay', millisecs))
  end
  def self.delay=(millisecs)
    self.delay(millisecs)
  end

  def self.fade?
    bool(tk_call_without_enc('::tooltip::tooltip', 'fade'))
  end
  def self.fade(mode)
    tk_call_without_enc('::tooltip::tooltip', 'fade', mode)
  end

  def self.disable
    tk_call_without_enc('::tooltip::tooltip', 'disable')
    false
  end
  def self.off
    self.disable
  end

  def self.enable
    tk_call_without_enc('::tooltip::tooltip', 'enable')
    true
  end
  def self.on
    self.enable
  end

  def self.register(widget, msg, keys=nil)
    if keys.kind_of?(Hash)
      args = hash_kv(keys) << msg
    else
      args = msg
    end
    tk_call('::tooltip::tooltip', widget.path, *args)
  end

  def self.erase(widget)
    tk_call_without_enc('::tooltip::tooltip', widget.path, '')
  end
end
