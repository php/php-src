#
#  tkextlib/iwidgets/panedwindow.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class Panedwindow < Tk::Itk::Widget
    end
  end
end

class Tk::Iwidgets::Panedwindow
  TkCommandNames = ['::iwidgets::panedwindow'.freeze].freeze
  WidgetClassName = 'Panedwindow'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  ####################################

  include TkItemConfigMethod

  def __item_cget_cmd(id)
    [self.path, 'panecget', id]
  end
  private :__item_cget_cmd

  def __item_config_cmd(id)
    [self.path, 'paneconfigure', id]
  end
  private :__item_config_cmd

  def tagid(tagOrId)
    if tagOrId.kind_of?(Tk::Itk::Component)
      tagOrId.name
    else
      #_get_eval_string(tagOrId)
      tagOrId
    end
  end

  alias panecget_tkstring itemcget_tkstring
  alias panecget itemcget
  alias panecget_strict itemcget_strict
  alias paneconfigure itemconfigure
  alias paneconfiginfo itemconfiginfo
  alias current_paneconfiginfo current_itemconfiginfo

  private :itemcget_tkstring, :itemcget, :itemcget_strict
  private :itemconfigure, :itemconfiginfo, :current_itemconfiginfo

  ####################################

  def __boolval_optkeys
    super() << 'showhandle'
  end
  private :__boolval_optkeys

  def add(tag=nil, keys={})
    if tag.kind_of?(Hash)
      keys = tag
      tag = nil
    end
    if tag
      tag = Tk::Itk::Component.new(self, tagid(tag))
    else
      tag = Tk::Itk::Component.new(self)
    end
    window(tk_call(@path, 'add', tagid(tag), *hash_kv(keys)))
    tag
  end

  def child_site_list
    list(tk_call(@path, 'childsite'))
  end

  def child_site(idx)
    window(tk_call(@path, 'childsite', index(idx)))
  end

  def delete(idx)
    tk_call(@path, 'delete', index(idx))
    self
  end

  def fraction(*percentages)
    tk_call(@path, 'fraction', *percentages)
    self
  end

  def hide(idx)
    tk_call(@path, 'hide', index(idx))
    self
  end

  def index(idx)
    number(tk_call(@path, 'index', tagid(idx)))
  end

  def insert(idx, tag=nil, keys={})
    if tag.kind_of?(Hash)
      keys = tag
      tag = nil
    end
    if tag
      tag = Tk::Itk::Component.new(self, tagid(tag))
    else
      tag = Tk::Itk::Component.new(self)
    end
    window(tk_call(@path, 'insert', index(idx), tagid(tag), *hash_kv(keys)))
    tag
  end

  def invoke(idx=nil)
    if idx
      tk_call(@path, 'invoke', index(idx))
    else
      tk_call(@path, 'invoke')
    end
    self
  end

  def reset
    tk_call(@path, 'reset')
    self
  end

  def show(idx)
    tk_call(@path, 'show', index(idx))
    self
  end
end
