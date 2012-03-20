#
#  tkextlib/tcllib/getstring.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * A dialog which consists of an Entry, OK, and Cancel buttons.
#

require 'tk'
require 'tk/entry'
require 'tkextlib/tcllib.rb'

# TkPackage.require('getstring', '0.1')
TkPackage.require('getstring')

module Tk::Tcllib
  class GetString_Dialog < TkWindow
    PACKAGE_NAME = 'getstring'.freeze
    def self.package_name
      PACKAGE_NAME
    end

    def self.package_version
      begin
        TkPackage.require('getstring')
      rescue
        ''
      end
    end
  end
end


class Tk::Tcllib::GetString_Dialog
  TkCommandNames = ['::getstring::tk_getString'.freeze].freeze
  WidgetClassName = 'TkSDialog'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def self.show(*args)
    dialog = self.new(*args)
    dialog.show
    [dialog.status, dialog.value]
  end
  def self.display(*args)
    self.show(*args)
  end

  def initialize(*args)   # args = (parent=nil, text='', keys=nil)
    keys = args.pop
    if keys.kind_of?(Hash)
      text = args.pop
      @keys = _symbolkey2str(keys)
      args.push(keys)
    else
      text = keys
      @keys = {}
    end
    if text
      @text = text.dup
    else
      @text = ''
    end

    @variable = TkVariable.new
    @status = nil

    super(*args)
  end

  def create_self(keys)
    # dummy
  end
  private :create_self

  def show
    @variable.value = ''
    @status = bool(tk_call(self.class::TkCommandNames[0],
                           @path, @variable, @text, *hash_kv(@keys)))
  end
  alias display show

  def status
    @status
  end

  def value
    @variable.value
  end

  def cget_strict(slot)
    slot = slot.to_s
    if slot == 'text'
      @text
    else
      @keys[slot]
    end
  end
  def cget(slot)
    cget_strict(slot)
  end

  def configure(slot, value=None)
    if slot.kind_of?(Hash)
      slot.each{|k, v| configure(k, v)}
    else
      slot = slot.to_s
      value = _symbolkey2str(value) if value.kind_of?(Hash)
      if value && value != None
        if slot == 'text'
          @text = value.to_s
        else
          @keys[slot] = value
        end
      else
        if slot == 'text'
          @text = ''
        else
          @keys.delete(slot)
        end
      end
    end
    self
  end

  def configinfo(slot = nil)
    if slot
      slot = slot.to_s
      [ slot, nil, nil, nil, ( (slot == 'text')? @text: @keys[slot] ) ]
    else
      @keys.collect{|k, v| [ k, nil, nil, nil, v ] }   \
      << [ 'text', nil, nil, nil, @text ]
    end
  end
end
