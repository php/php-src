#
#  tkextlib/tcllib/swaplist.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * A dialog which allows a user to move options between two lists
#

require 'tk'
require 'tkextlib/tcllib.rb'

# TkPackage.require('swaplist', '0.1')
TkPackage.require('swaplist')

module Tk::Tcllib
  class Swaplist_Dialog < TkWindow
    PACKAGE_NAME = 'swaplist'.freeze
    def self.package_name
      PACKAGE_NAME
    end

    def self.package_version
      begin
        TkPackage.require('swaplist')
      rescue
        ''
      end
    end
  end
end


class Tk::Tcllib::Swaplist_Dialog
  TkCommandNames = ['::swaplist::swaplist'.freeze].freeze
  WidgetClassName = 'Swaplist'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def self.show(*args)
    dialog = self.new(*args)
    dialog.show
    [dialog.status, dialog.value]
  end
  def self.display(*args)
    self.show(*args)
  end

  def initialize(*args)
    # args = (parent=nil, complete_list=[], selected_list=[], keys=nil)
    keys = args.pop
    if keys.kind_of?(Hash)
      @selected_list = args.pop
      @complete_list = args.pop
      @keys = _symbolkey2str(keys)
      args.push(keys)
    else
      @selected_list = keys
      @complete_list = args.pop
      @keys = {}
    end

    @selected_list = [] unless @selected_list
    @complete_list = [] unless @complete_list

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
                           @path, @variable,
                           @complete_list, @selected_list,
                           *hash_kv(@keys)))
  end
  alias display show

  def status
    @status
  end

  def value
    @variable.list
  end
  alias selected value

  def cget_strict(slot)
    slot = slot.to_s
    if slot == 'complete_list'
      @complete_list
    elsif slot == 'selected_list'
      @selected_list
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
        if slot == 'complete_list'
          @complete_list = value
        elsif slot == 'selected_list'
          @selected_list = value
        else
          @keys[slot] = value
        end
      else
        if slot == 'complete_list'
          @complete_list = []
        elsif slot == 'selected_list'
          @selected_list = []
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
      if slot == 'complete_list'
        [ slot, nil, nil, nil, @complete_list ]
      elsif slot == 'selected_list'
        [ slot, nil, nil, nil, @selected_list ]
      else
        [ slot, nil, nil, nil, @keys[slot] ]
      end
    else
      @keys.collect{|k, v| [ k, nil, nil, nil, v ] }           \
      << [ 'complete_list', nil, nil, nil, @complete_list ]   \
      << [ 'selected_list', nil, nil, nil, @selected_list ]
    end
  end
end
