#
#  tkextlib/bwidget/dropsite.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/bwidget.rb'

module Tk
  module BWidget
    module DropSite
    end
  end
end

module Tk::BWidget::DropSite
  include Tk
  extend Tk

  def self.include(klass, type)
    tk_call('DropSite::include', klass, type)
  end

  def self.register(path, keys={})
    tk_call('DropSite::register', path, *hash_kv(keys))
  end

  def self.set_cursor(cursor)
    tk_call('DropSite::setcursor', cursor)
  end

  def self.set_drop(path, subpath, dropover, drop, force=None)
    tk_call('DropSite::setdrop', path, subpath, dropover, drop, force)
  end

  def self.set_operation(op)
    tk_call('DropSite::setoperation', op)
  end
end
