#
#  tkextlib/tcllib/history.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * Provides a history for Entry widgets
#

require 'tk'
require 'tk/entry'
require 'tkextlib/tcllib.rb'

# TkPackage.require('history', '0.1')
TkPackage.require('history')

module Tk::Tcllib
  module History
    PACKAGE_NAME = 'history'.freeze
    def self.package_name
      PACKAGE_NAME
    end

    def self.package_version
      begin
        TkPackage.require('history')
      rescue
        ''
      end
    end
  end
end

module Tk::Tcllib::History
  extend TkCore

  def self.init(entry, length=None)
    tk_call_without_enc('::history::init', entry.path, length)
    entry.extend(self)  # add methods to treat history to the entry widget
  end

  def self.remove(entry)
    tk_call_without_enc('::history::remove', entry.path)
    entry
  end

  def history_remove
    tk_call_without_enc('::history::remove', @path)
    self
  end

  def history_add(text)
    tk_call('::history::add', @path, text)
    self
  end

  def history_get
    simplelist(tk_call_without_enc('::history::get', @path))
  end

  def history_clear
    tk_call_without_enc('::history::clear', @path)
    self
  end

  def history_configure(opt, value)
    tk_call('::history::configure', @path, opt, value)
    self
  end

  def history_configinfo(opt)
    tk_call('::history::configure', @path, opt)
  end
end
