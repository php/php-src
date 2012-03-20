#
#  tkextlib/bwidget/selectcolor.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/bwidget.rb'
require 'tkextlib/bwidget/messagedlg'

module Tk
  module BWidget
    class SelectColor < Tk::BWidget::MessageDlg
      class Dialog < Tk::BWidget::SelectColor
      end
      class Menubutton < Tk::Menubutton
      end
      MenuButton = Menubutton
    end
  end
end

class Tk::BWidget::SelectColor
  extend Tk

  TkCommandNames = ['SelectColor'.freeze].freeze
  WidgetClassName = 'SelectColor'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def dialog(keys={})
    newkeys = @keys.dup
    newkeys.update(_symbolkey2str(keys))
    tk_call('SelectColor::dialog', @path, *hash_kv(newkeys))
  end

  def menu(*args)
    if args[-1].kind_of?(Hash)
      keys = args.pop
    else
      keys = {}
    end
    place = args.flatten
    newkeys = @keys.dup
    newkeys.update(_symbolkey2str(keys))
    tk_call('SelectColor::menu', @path, place, *hash_kv(newkeys))
  end

  def self.set_color(idx, color)
    tk_call('SelectColor::setcolor', idx, color)
  end
end

class Tk::BWidget::SelectColor::Dialog
  def create_self(keys)
    super(keys)
    @keys['type'] = 'dialog'
  end

  def create
    @keys['type'] = 'dialog'  # 'dialog' type returns color
    tk_call(Tk::BWidget::SelectColor::TkCommandNames[0],
            @path, *hash_kv(@keys))
  end
end

class Tk::BWidget::SelectColor::Menubutton
  def create_self(keys)
    keys = {} unless keys
    keys = _symbolkey2str(keys)
    keys['type'] = 'menubutton'  # 'toolbar' type returns widget path
    window(tk_call(Tk::BWidget::SelectColor::TkCommandNames[0],
                   @path, *hash_kv(keys)))
  end
end
