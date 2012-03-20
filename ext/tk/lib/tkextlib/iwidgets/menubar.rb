#
#  tkextlib/iwidgets/menubar.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class Menubar < Tk::Itk::Widget
    end
  end
end

class Tk::Iwidgets::Menubar
  TkCommandNames = ['::iwidgets::menubar'.freeze].freeze
  WidgetClassName = 'Menubar'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __strval_optkeys
    super() << 'menubuttons'
  end
  private :__strval_optkeys

  def __tkvariable_optkeys
    super() << 'helpvariable'
  end
  private :__tkvariable_optkeys

  ####################################

  include TkItemConfigMethod

  def __item_cget_cmd(id)
    [self.path, 'menucget', id]
  end
  private :__item_cget_cmd

  def __item_config_cmd(id)
    [self.path, 'menuconfigure', id]
  end
  private :__item_config_cmd

  def __item_strval_optkeys(id)
    super(id) << 'selectcolor'
  end
  private :__item_strval_optkeys

  def __item_tkvariable_optkeys(id)
    super(id) << 'helpstr'
  end
  private :__item_tkvariable_optkeys

  def tagid(tagOrId)
    if tagOrId.kind_of?(Tk::Itk::Component)
      tagOrId.name
    else
      #_get_eval_string(tagOrId)
      tagOrId
    end
  end

  alias menucget_tkstring itemcget_tkstring
  alias menucget itemcget
  alias menucget_strict itemcget_strict
  alias menuconfigure itemconfigure
  alias menuconfiginfo itemconfiginfo
  alias current_menuconfiginfo current_itemconfiginfo

  private :itemcget_tkstring, :itemcget, :itemcget_strict
  private :itemconfigure, :itemconfiginfo, :current_itemconfiginfo

  ####################################

  def __methodcall_optkeys
    {'menubuttons'=>'menubuttons'}
  end

  def menubuttons(val = nil)
    unless val
      return tk_call(@path, 'cget', '-menubuttons')
    end

    tk_call(@path, 'configure', '-menubuttons', _parse_menu_spec(val))
    self
  end

  def _parse_menu_spec(menu_spec)
    ret = ''
    menu_spec.each{|spec|
      next unless spec

      if spec.kind_of?(Hash)
        args = [spec]
        type = 'options'
      else
        type, *args = spec
      end

      type = type.to_s
      case type
      when 'options'
        keys = args[0]
        ary = [type]
        ary.concat(hash_kv(keys))
        ret << array2tk_list(ary) << "\n"

      when 'menubutton', 'cascade'
        name, keys = args
        if keys
          ary = [type, name]
          keys = _symbolkey2str(keys)
          keys['menu'] = _parse_menu_spec(keys['menu']) if keys.key?('menu')
          ary.concat(hash_kv(keys))
          ret << array2tk_list(ary) << "\n"
        else
          ret << array2tk_list([type, name]) << "\n"
        end

      else
        name, keys = args
        if keys
          ary = [type, name]
          ary.concat(hash_kv(keys))
          ret << array2tk_list(ary) << "\n"
        else
          ret << array2tk_list([type, name]) << "\n"
        end
      end
    }
    ret
  end

  ####################################

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
    keys = _symbolkey2str(keys)
    keys['menu'] = _parse_menu_spec(keys['menu']) if keys.key?('menu')
    tk_call(@path, 'add', type, tagid(tag), *hash_kv(keys))
    tag
  end

  def delete(path1, path2=nil)
    if path2
    else
      tk_call(@path, 'delete', index(idx))
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
    keys = _symbolkey2str(keys)
    keys['menu'] = _parse_menu_spec(keys['menu']) if keys.key?('menu')
    tk_call(@path, 'insert', index(idx), type, tagid(tag), *hash_kv(keys))
    tag
  end

  def invoke(idx)
    tk_call(@path, 'invoke', index(idx))
    self
  end

  def menupath(pat)
    if (win = tk_call(@path, 'path', pat)) == '-1'
      return nil
    end
    window(win)
  end
  def menupath_glob(pat)
    if (win = tk_call(@path, 'path', '-glob', pat)) == '-1'
      return nil
    end
    window(win)
  end
  def menupath_tclregexp(pat)
    if (win = tk_call(@path, 'path', '-regexp', pat)) == '-1'
      return nil
    end
    window(win)
  end

  def type(path)
    tk_call(@path, 'type', path)
  end

  def yposition(path)
    number(tk_call(@path, 'yposition', path))
  end
end
