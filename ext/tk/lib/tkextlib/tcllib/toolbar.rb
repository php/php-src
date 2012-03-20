#
#  tkextlib/tcllib/toolbar.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * toolbar widget
#

require 'tk'
require 'tkextlib/tcllib.rb'

# TkPackage.require('widget::toolbar', '1.2')
TkPackage.require('widget::toolbar')

module Tk::Tcllib
  module Widget
    class Toolbar < TkWindow
      PACKAGE_NAME = 'widget::toolbar'.freeze
      def self.package_name
        PACKAGE_NAME
      end

      def self.package_version
        begin
          TkPackage.require('widget::toolbar')
        rescue
          ''
        end
      end
    end

    module ToolbarItemConfig
      include TkItemConfigMethod
    end
  end
end


class Tk::Tcllib::Widget::ToolbarItem < TkObject
  include TkTreatTagFont

  ToolbarItemID_TBL = TkCore::INTERP.create_table

  TkCore::INTERP.init_ip_env{
    TTagID_TBL.mutex.synchronize{ TTagID_TBL.clear }
  }

  def ToolbarItem.id2obj(tbar, id)
    tpath = tbar.path
    ToolbarItemID_TBL.mutex.synchronize{
      if ToolbarItemID_TBL[tpath]
        ToolbarItemID_TBL[tpath][id]? ToolbarItemID_TBL[tpath][id]: id
      else
        id
      end
    }
  end

  def initaialize(parent, *args)
    @parent = @t = parent
    @tpath = parent.path

    @path = @id = @t.tk_send('add', *args)
    # A same id is rejected by the Tcl function.

    ToolbarItemID_TBL.mutex.synchronize{
      ToolbarItemID_TBL[@id] = self
      ToolbarItemID_TBL[@tpath] = {} unless ToolbarItemID_TBL[@tpath]
      ToolbarItemID_TBL[@tpath][@id] = self
    }
  end

  def [](key)
    cget key
  end

  def []=(key,val)
    configure key, val
    val
  end

  def cget_tkstring(option)
    @t.itemcget_tkstring(@id, option)
  end
  def cget(option)
    @t.itemcget(@id, option)
  end
  def cget_strict(option)
    @t.itemcget_strict(@id, option)
  end

  def configure(key, value=None)
    @t.itemconfigure(@id, key, value)
    self
  end

  def configinfo(key=nil)
    @t.itemconfiginfo(@id, key)
  end

  def current_configinfo(key=nil)
    @t.current_itemconfiginfo(@id, key)
  end

  def delete
    @t.delete(@id)
  end

  def itemid
    @t.itemid(@id)
  end

  def remove
    @t.remove(@id)
  end
  def remove_with_destroy
    @t.remove_with_destroy(@id)
  end
end

class Tk::Tcllib::Widget::Toolbar
  include Tk::Tcllib::Widget::ToolbarItemConfig

  TkCommandNames = ['::widget::toolbar'.freeze].freeze

  def __destroy_hook__
    Tk::Tcllib::Widget::ToolbarItem::ToolbarItemID_TBL.mutex.synchronize{
      Tk::Tcllib::Widget::ToolbarItem::ToolbarItemID_TBL.delete(@path)
    }
  end

  def create_self(keys)
    if keys and keys != None
      tk_call_without_enc(self.class::TkCommandNames[0], @path,
                          *hash_kv(keys, true))
    else
      tk_call_without_enc(self.class::TkCommandNames[0], @path)
    end
  end
  private :create_self

  def getframe
    window(tk_send('getframe'))
  end
  alias get_frame getframe

  def add(*args)
    Tk::Tcllib::Widget::Toolbar.new(self, *args)
  end

  def itemid(item)
    window(tk_send('itemid'))
  end

  def items(pattern)
    tk_split_simplelist(tk_send('items', pattern)).map{|id|
      Tk::Tcllib::Widget::ToolbarItem.id2obj(self, id)
    }
  end

  def remove(*items)
    tk_send('remove', *items)
    self
  end

  def remove_with_destroy(*items)
    tk_send('remove', '-destroy', *items)
    self
  end

  def delete(*items)
    tk_send('delete', *items)
    self
  end
end
