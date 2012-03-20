#
#  tkextlib/iwidgets/toolbar.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class Toolbar < Tk::Itk::Widget
    end
  end
end

class Tk::Iwidgets::Toolbar
  TkCommandNames = ['::iwidgets::toolbar'.freeze].freeze
  WidgetClassName = 'Toolbar'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __tkvariable_optkeys
    super() << 'helpvariable'
  end
  private :__tkvariable_optkeys

  ####################################

  include TkItemConfigMethod

  def __item_cget_cmd(id)
    [self.path, 'itemcget', self.index(id)]
  end
  private :__item_cget_cmd

  def __item_config_cmd(id)
    [self.path, 'itemconfigure', self.index(id)]
  end
  private :__item_config_cmd

  def __item_strval_optkeys(id)
    super(id) << 'helpstr' << 'balloonstr'
  end
  private :__item_strval_optkeys

  def tagid(tagOrId)
    if tagOrId.kind_of?(Tk::Itk::Component)
      tagOrId.name
    else
      #_get_eval_string(tagOrId)
      tagOrId
    end
  end

  ####################################

  def __strval_optkeys
    super() << 'balloonbackground' << 'balloonforeground'
  end
  private :__strval_optkeys

  def __tkvariable_optkeys
    super() << 'helpvariable'
  end
  private :__tkvariable_optkeys

  def __font_optkeys
    super() << 'balloonfont'
  end
  private :__font_optkeys

  def add(type, tag=nil, keys={})
    if tag.kind_of?(Hash)
      keys = tag
      tag = nil
    end
    if tag
      tag = Tk::Itk::Component.new(self, tagid(tag))
    else
      tag = Tk::Itk::Component.new(self)
    end
    window(tk_call(@path, 'add', type, tagid(tag), *hash_kv(keys)))
    tag
  end

  def delete(idx1, idx2=nil)
    if idx2
      tk_call(@path, 'delete', index(idx1), index(idx2))
    else
      tk_call(@path, 'delete', index(idx1))
    end
    self
  end

  def index(idx)
    number(tk_call(@path, 'index', tagid(idx)))
  end

  def insert(idx, type, tag=nil, keys={})
    if tag.kind_of?(Hash)
      keys = tag
      tag = nil
    end
    if tag
      tag = Tk::Itk::Component.new(self, tagid(tag))
    else
      tag = Tk::Itk::Component.new(self)
    end
    window(tk_call(@path, 'insert', index(idx), type,
                   tagid(tag), *hash_kv(keys)))
    tag
  end
end
