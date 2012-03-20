#
#  tkextlib/tcllib/crosshair.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * Crosshairs for Tk canvas
#

require 'tk'
require 'tkextlib/tcllib.rb'

# TkPackage.require('crosshair', '1.0.2')
TkPackage.require('crosshair')

module Tk::Tcllib
  module Crosshair
    PACKAGE_NAME = 'crosshair'.freeze
    def self.package_name
      PACKAGE_NAME
    end

    def self.package_version
      begin
        TkPackage.require('crosshair')
      rescue
        ''
      end
    end
  end
end

module Tk::Tcllib::Crosshair
  include TkCore
  TkCommandNames = ['::crosshair::crosshair'.freeze].freeze

  def self.crosshair(w, keys={})
    Tk.tk_call('::crosshair::crosshair', w, *hash_kv(keys))
    w
  end
  def self.on(w, keys={})
    self.crosshair(w, keys)
  end

  def self.off(w)
    Tk.tk_call('::crosshair::off', w)
    w
  end

  def self.track_on(w, &b)
    Tk.tk_call('::crosshair::track_on', w, b)
    w
  end

  def self.track_off(w)
    Tk.tk_call('::crosshair::track_off', w)
    w
  end
end

class << Tk::Tcllib::Crosshair
  include TkComm
  include TkCanvasItemConfig

  def __item_methodcall_optkeys(id)
    {}
  end
  private :__item_methodcall_optkeys

  def __item_config_cmd(id)
    # maybe need to override
    ['::crosshair::configure', id]
  end
  private :__item_config_cmd

  private :itemcget_tkstring, :itemcget, :itemcget_strict
  private :itemconfigure, :itemconfiginfo, :current_itemconfiginfo

  def confugure(w, slot, value=None)
    itemconfigure(w, slot, value)
  end
  def confuginfo(w, slot = nil)
    itemconfiginfo(w, slot)
  end
  def current_configinfo(w, slot = nil)
    current_itemconfiginfo(w, slot)
  end
  def cget(w, slot)
    current_itemconfiginfo(w, slot).values[0]
  end
end

module Tk::Tcllib::Crosshair
  def crosshair_on(keys={})
    Tk::Tcllib::Crosshair.on(self, keys)
  end
  def crosshair_off
    Tk::Tcllib::Crosshair.off(self)
  end
  def crosshair_track_on(&b)
    Tk::Tcllib::Crosshair.track_on(self, &b)
  end
  def crosshair_track_off
    Tk::Tcllib::Crosshair.track_off(self)
  end
  def crosshair_configure(*args)
    Tk::Tcllib::Crosshair.configure(self, *args)
  end
  def crosshair_configinfo(slot = nil)
    Tk::Tcllib::Crosshair.configinfo(self, slot)
  end
  def crosshair_current_configinfo(slot = nil)
    Tk::Tcllib::Crosshair.current_configinfo(self, slot)
  end
  def crosshair_cget(slot)
    Tk::Tcllib::Crosshair.cget(self, slot)
  end
end
