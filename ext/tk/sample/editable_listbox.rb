#
# Tk::RbWidget::Editable_Listbox class
#
#   When "DoubleClick-1" on a listbox item, the entry box is opend on the
#   item. And when hit "Return" key on the entry box after modifying the
#   text, the entry box is closed and the item is changed. Or when hit
#   "Escape" key, the entry box is closed without modification.
#
#                              by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'

module Tk
  module RbWidget
    class Editable_Listbox < TkListbox
    end
  end
end


class Tk::RbWidget::Editable_Listbox < TkListbox
  #------------------------------------
  BindTag = TkBindTag.new_by_name(self.to_s.gsub(/::/, '#'))

  BindTag.bind('FocusIn', :widget){|w|
    w.instance_eval{
      if idx = @ebox.pos
        see(idx) if bbox(idx).empty?
        @ebox.focus(true)
      end
    }
  }

  BindTag.bind('Double-1', :widget, :y){|w, y|
    w.instance_eval{ _ebox_placer(nearest(y)) }
  }

  BindTag.bind('Return', :widget){|w|
    w.instance_eval{
      if idx = index(:active)
        _ebox_placer(idx)
      end
    }
  }
  #------------------------------------

  def configure(*args)
    ret = super

    case cget(:state)
    when 'normal'
      # do nothing
    when 'disabled'
      _ebox_erase
    else # unknown
      # do nothing

    end

    ret
  end

  def _ebox_move(idx)
    return nil if cget(:state) == 'disabled'
    x, y, w, h = bbox(idx)
    return nil unless y && h
    @ebox.place(:x => 0, :relwidth => 1.0,
                :y => y - selectborderwidth,
                :height => h + 2 * selectborderwidth)
    @ebox.pos = idx
    @ebox.focus
  end

  def _ebox_placer(idx)
    return nil unless _ebox_move(idx)
    @ebox.value = listvariable.list[idx]
    @ebox.xview_moveto(self.xview[0])
  end

  def _ebox_erase
    @ebox.place_forget
    @ebox.pos = nil
  end
  private :_ebox_move, :_ebox_placer, :_ebox_erase

  def _setup_ebox_bindings
    # bindings for entry
    @ebox.bind('Return'){
      list = listvariable.list
      list[@ebox.pos] = @ebox.value if @ebox.pos
      listvariable.value = list
      _ebox_erase
      focus
    }

    @ebox.bind('Escape'){ _ebox_erase }
  end
  def _setup_listbox_bindings
    # bindings for listbox
    tags = bindtags
    bindtags(tags.insert(tags.index(self) + 1, self.class::BindTag))
  end
  private :_setup_ebox_bindings,  :_setup_listbox_bindings

  def yview(*args)
    if !@ebox.pos || bbox(@ebox.pos).empty?
      @ebox.place_forget
    else
      _ebox_move(@ebox.pos)
    end
    super
  end

  def create_self(keys)
    super(keys)

    unless self.listvariable
      self.listvariable = TkVariable.new(self.get(0, :end))
    end

    @ebox = TkEntry.new(self){
      @pos = nil
      def self.pos; @pos; end
      def self.pos=(idx); @pos = idx; end
    }

    _setup_ebox_bindings
    _setup_listbox_bindings
  end
end

if $0 == __FILE__
  #lbox0 = TkListbox.new.pack(:side=>:left)
  #lbox0.insert(:end,     0,1,2,3,4,5,6,7,8,9,0,1,2,3)

  scr = TkScrollbar.new.pack(:side=>:right, :fill=>:y)

  lbox1 = Tk::RbWidget::Editable_Listbox.new.pack(:side=>:left)
  lbox2 = Tk::RbWidget::Editable_Listbox.new.pack(:side=>:left)

  scr.assign(lbox1, lbox2)

  lbox1.insert(:end, *%w(a b c d e f g h i j k l m n))
  lbox2.insert(:end,     0,1,2,3,4,5,6,7,8,9,0,1,2,3)


  Tk.mainloop
end
