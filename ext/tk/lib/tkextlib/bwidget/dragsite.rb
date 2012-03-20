#
#  tkextlib/bwidget/dragsite.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/bwidget.rb'

module Tk
  module BWidget
    module DragSite
    end
  end
end

module Tk::BWidget::DragSite
  include Tk
  extend Tk

  def self.include(klass, type, event)
    tk_call('DragSite::include', klass, type, event)
  end

  def self.register(path, keys={})
    tk_call('DragSite::register', path, *hash_kv(keys))
  end

  def self.set_drag(path, subpath, initcmd, endcmd, force=None)
    tk_call('DragSite::setdrag', path, subpath, initcmd, endcmd, force)
  end
end
