#
#  tkextlib/iwidgets/tabnotebook.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class Tabnotebook < Tk::Itk::Widget
    end
  end
end

class Tk::Iwidgets::Tabnotebook
  TkCommandNames = ['::iwidgets::tabnotebook'.freeze].freeze
  WidgetClassName = 'Tabnotebook'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  ####################################

  include TkItemConfigMethod

  def __item_cget_cmd(id)
    [self.path, 'pagecget', id]
  end
  private :__item_cget_cmd

  def __item_config_cmd(id)
    [self.path, 'pageconfigure', id]
  end
  private :__item_config_cmd

  def __item_strval_optkeys(id)
    super(id) << 'tabbackground' << 'tabforeground'
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

  alias pagecget_tkstring itemcget_tkstring
  alias pagecget itemcget
  alias pagecget_strict itemcget_strict
  alias pageconfigure itemconfigure
  alias pageconfiginfo itemconfiginfo
  alias current_pageconfiginfo current_itemconfiginfo

  private :itemcget_tkstring, :itemcget, :itemcget_strict
  private :itemconfigure, :itemconfiginfo, :current_itemconfiginfo

  ####################################

  def __boolval_optkeys
    super() << 'auto' << 'equaltabs' << 'raiseselect' << 'tabborders'
  end
  private :__boolval_optkeys

  def __strval_optkeys
    super() << 'backdrop' << 'tabbackground' << 'tabforeground'
  end
  private :__strval_optkeys

  def initialize(*args)
    super(*args)
    @tabset = self.component_widget('tabset')
  end

  def add(keys={})
    window(tk_call(@path, 'add', *hash_kv(keys)))
  end

  def child_site_list
    list(tk_call(@path, 'childsite'))
  end

  def child_site(idx)
    window(tk_call(@path, 'childsite', index(idx)))
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
    #number(tk_call(@path, 'index', tagid(idx)))
    @tabset.index(tagid(idx))
  end

  def insert(idx, keys={})
    window(tk_call(@path, 'insert', index(idx), *hash_kv(keys)))
  end

  def next
    tk_call(@path, 'next')
    self
  end

  def prev
    tk_call(@path, 'prev')
    self
  end

  def select(idx)
    tk_call(@path, 'select', index(idx))
    self
  end

  def show_tab(idx)
    @tabset.show_tab(idx)
    self
  end

  def scrollcommand(cmd=Proc.new)
    configure_cmd 'scrollcommand', cmd
    self
  end
  alias xscrollcommand scrollcommand
  alias yscrollcommand scrollcommand

  def xscrollbar(bar=nil)
    if bar
      @scrollbar = bar
      @scrollbar.orient 'horizontal'
      self.scrollcommand {|*arg| @scrollbar.set(*arg)}
      @scrollbar.command {|*arg| self.xview(*arg)}
      Tk.update  # avoid scrollbar trouble
    end
    @scrollbar
  end
  def yscrollbar(bar=nil)
    if bar
      @scrollbar = bar
      @scrollbar.orient 'vertical'
      self.scrollcommand {|*arg| @scrollbar.set(*arg)}
      @scrollbar.command {|*arg| self.yview(*arg)}
      Tk.update  # avoid scrollbar trouble
    end
    @scrollbar
  end
  alias scrollbar yscrollbar

  def view(*index)
    if index.size == 0
      idx = num_or_str(tk_send_without_enc('view'))
      if idx.kind_of?(Fixnum) && idx < 0
        nil
      else
        idx
      end
    else
      tk_send_without_enc('view', *index)
      self
    end
  end
  alias xview view
  alias yview view

  def view_moveto(*index)
    view('moveto', *index)
  end
  alias xview_moveto view_moveto
  alias yview_moveto view_moveto
  def view_scroll(index, what='pages')
    view('scroll', index, what)
  end
  alias xview_scroll view_scroll
  alias yview_scroll view_scroll
end
