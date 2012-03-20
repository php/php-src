#
#  tkextlib/iwidgets/dialogshell.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class Dialogshell < Tk::Iwidgets::Shell
    end
  end
end

class Tk::Iwidgets::Dialogshell
  TkCommandNames = ['::iwidgets::dialogshell'.freeze].freeze
  WidgetClassName = 'Dialogshell'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  ####################################

  include TkItemConfigMethod

  def __item_cget_cmd(id)
    [self.path, 'buttoncget', id]
  end
  private :__item_cget_cmd

  def __item_config_cmd(id)
    [self.path, 'buttonconfigure', id]
  end
  private :__item_config_cmd

  def __item_boolval_optkeys(id)
    super(id) << 'defaultring'
  end
  private :__item_boolval_optkeys

  def tagid(tagOrId)
    if tagOrId.kind_of?(Tk::Itk::Component)
      tagOrId.name
    else
      #_get_eval_string(tagOrId)
      tagOrId
    end
  end

  alias buttoncget_tkstring itemcget_tkstring
  alias buttoncget itemcget
  alias buttoncget_strict itemcget_strict
  alias buttonconfigure itemconfigure
  alias buttonconfiginfo itemconfiginfo
  alias current_buttonconfiginfo current_itemconfiginfo

  private :itemcget_tkstring, :itemcget, :itemcget_strict
  private :itemconfigure, :itemconfiginfo, :current_itemconfiginfo

  ####################################

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
    tk_call(@path, 'add', tagid(tag), *hash_kv(keys))
    tag
  end

  def default(idx)
    tk_call(@path, 'default', index(idx))
    self
  end

  def delete(idx)
    tk_call(@path, 'delete', index(idx))
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
    tk_call(@path, 'insert', index(idx), tagid(tag), *hash_kv(keys))
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

  def show(idx)
    tk_call(@path, 'show', index(idx))
    self
  end
end
