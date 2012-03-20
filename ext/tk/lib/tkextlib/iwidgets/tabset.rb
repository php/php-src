#
#  tkextlib/iwidgets/tabset.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class Tabset < Tk::Itk::Widget
    end
  end
end

class Tk::Iwidgets::Tabset
  TkCommandNames = ['::iwidgets::tabset'.freeze].freeze
  WidgetClassName = 'Tabset'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  ####################################

  include TkItemConfigMethod

  def __item_cget_cmd(id)
    [self.path, 'tabcget', id]
  end
  private :__item_cget_cmd

  def __item_config_cmd(id)
    [self.path, 'tabconfigure', id]
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

  alias tabcget_tkstring itemcget_tkstring
  alias tabcget itemcget
  alias tabcget_strict itemcget_strict
  alias tabconfigure itemconfigure
  alias tabconfiginfo itemconfiginfo
  alias current_tabconfiginfo current_itemconfiginfo

  private :itemcget_tkstring, :itemcget, :itemcget_strict
  private :itemconfigure, :itemconfiginfo, :current_itemconfiginfo

  ####################################

  def __boolval_optkeys
    super() << 'equaltabs' << 'raiseselect' << 'tabborders'
  end
  private :__boolval_optkeys

  def __strval_optkeys
    super() << 'backdrop'
  end
  private :__strval_optkeys

  def add(keys={})
    window(tk_call(@path, 'add', *hash_kv(keys)))
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
    if index(idx) == 0
      self.start = 0
      return
    end

    reutrn unless @canvas ||= self.winfo_children[0]

    delta = 1 if (delta = cget(:gap)) == 'overlap' ||
                   (delta = self.winfo_pixels(delta) + 1) <= 0

    case cget(:tabpos)
    when 's', 'n'
      if (head = tabcget(idx, :left)) < 0
        self.start -= head
        return
      end
      tabs_size = @canvas.winfo_width
      tab_start, tab_end = @canvas .
        find_overlapping(head, 0, head + delta, @canvas.winfo_height) .
        find_all{|id| @canvas.itemtype(id) == TkcPolygon} .
        map!{|id| bbox = @canvas.bbox(id); [bbox[0], bbox[2]]} . max

    when 'e', 'w'
      if (head = tabcget(idx, :top)) < 0
        self.start -= head
        return
      end
      tabs_size = @canvas.winfo_height
      tab_start, tab_end = @canvas .
        find_overlapping(0, head, @canvas.winfo_width, head + delta) .
        find_all{|id| @canvas.itemtype(id) == TkcPolygon} .
        map!{|id| bbox = @canvas.bbox(id); [bbox[1], bbox[3]]} . max
    end

    if (size = tab_end - tab_start + 1) > tabs_size
      self.start -= tab_start
    elsif head + size > tabs_size
      self.start -= head + size - tabs_size
    end

    self
  end
end
