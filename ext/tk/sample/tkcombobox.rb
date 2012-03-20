#
#  tkcombobox.rb : auto scrollbox & combobox
#
#                         by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'

module Tk
  module RbWidget
    class AutoScrollListbox < TkListbox
    end
    class Combobox < TkEntry
    end
  end
end

class Tk::RbWidget::AutoScrollListbox
  include TkComposite

  @@up_bmp = TkBitmapImage.new(:data=><<EOD)
#define up_arrow_width 9
#define up_arrow_height 9
static unsigned char up_arrow_bits[] = {
   0x00, 0x00, 0x10, 0x00, 0x10, 0x00, 0x38, 0x00, 0x38, 0x00, 0x7c, 0x00,
   0x7c, 0x00, 0xfe, 0x00, 0x00, 0x00};
EOD

  @@down_bmp = TkBitmapImage.new(:data=><<EOD)
#define up_arrow_width 9
#define up_arrow_height 9
static unsigned char down_arrow_bits[] = {
   0x00, 0x00, 0xfe, 0x00, 0x7c, 0x00, 0x7c, 0x00, 0x38, 0x00, 0x38, 0x00,
   0x10, 0x00, 0x10, 0x00, 0x00, 0x00};
EOD

  ############################
  private
  ############################
  def initialize_composite(keys={})
    keys = _symbolkey2str(keys)

    @initwait = keys.delete('startwait'){300}
    @interval = keys.delete('interval'){150}
    @initwait -= @interval
    @initwait = 0 if @initwait < 0

    @lbox = TkListbox.new(@frame, :borderwidth=>0)
    @path = @lbox.path
    TkPack.propagate(@lbox, false)

    @scr = TkScrollbar.new(@frame, :width=>10)

    @lbox.yscrollcommand(proc{|*args| @scr.set(*args); _config_proc})
    @scr.command(proc{|*args| @lbox.yview(*args); _config_proc})

    @up_arrow   = TkLabel.new(@lbox, :image=>@@up_bmp,
                              :relief=>:raised, :borderwidth=>1)
    @down_arrow = TkLabel.new(@lbox, :image=>@@down_bmp,
                              :relief=>:raised, :borderwidth=>1)

    _init_binding

    @lbox.pack(:side=>:left, :fill=>:both, :expand=>:true)

    delegate('DEFAULT', @lbox)
    delegate('background', @frame, @scr)
    delegate('activebackground', @scr)
    delegate('troughcolor', @scr)
    delegate('repeatdelay', @scr)
    delegate('repeatinterval', @scr)
    delegate('relief', @frame)
    delegate('borderwidth', @frame)

    delegate_alias('arrowrelief', 'relief', @up_arrow, @down_arrow)
    delegate_alias('arrowborderwidth', 'borderwidth', @up_arrow, @down_arrow)

    scrollbar(keys.delete('scrollbar')){false}

    configure keys unless keys.empty?
  end

  def _show_up_arrow
    unless @up_arrow.winfo_mapped?
      @up_arrow.pack(:side=>:top, :fill=>:x)
    end
  end

  def _show_down_arrow
    unless @down_arrow.winfo_mapped?
      @down_arrow.pack(:side=>:bottom, :fill=>:x)
    end
  end

  def _set_sel(idx)
      @lbox.activate(idx)
      @lbox.selection_clear(0, 'end')
      @lbox.selection_set(idx)
  end

  def _check_sel(cidx, tidx = nil, bidx = nil)
    _set_sel(cidx)
    unless tidx
      tidx = @lbox.nearest(0)
      tidx += 1 if tidx > 0
    end
    unless bidx
      bidx = @lbox.nearest(10000)
      bidx -= 1 if bidx < @lbox.index('end') - 1
    end
    if cidx > bidx
      _set_sel(bidx)
    end
    if cidx < tidx
      _set_sel(tidx)
    end
  end

  def _up_proc
    cidx = @lbox.curselection[0]
    idx = @lbox.nearest(0)
    if idx >= 0
      @lbox.see(idx - 1)
      _set_sel(idx)
      @up_arrow.pack_forget if idx == 1
      @up_timer.stop if idx == 0
      _show_down_arrow if @lbox.bbox('end') == []
    end
    if cidx && cidx > 0 && (idx == 0 || cidx == @lbox.nearest(10000))
      _set_sel(cidx - 1)
    end
  end

  def _down_proc
    cidx = @lbox.curselection[0]
    eidx = @lbox.index('end') - 1
    idx = @lbox.nearest(10000)
    if idx <= eidx
      @lbox.see(idx + 1)
      _set_sel(cidx + 1) if cidx < eidx
      @down_arrow.pack_forget if idx + 1 == eidx
      @down_timer.stop if idx == eidx
      _show_up_arrow if @lbox.bbox(0) == []
    end
    if cidx && cidx < eidx && (eidx == idx || cidx == @lbox.nearest(0))
      _set_sel(cidx + 1)
    end
  end

  def _key_UP_proc
    cidx = @lbox.curselection[0]
    _set_sel(cidx = @lbox.index('activate')) unless cidx
    cidx -= 1
    if cidx == 0
      @up_arrow.pack_forget
    elsif cidx == @lbox.nearest(0)
      @lbox.see(cidx - 1)
    end
  end

  def _key_DOWN_proc
    cidx = @lbox.curselection[0]
    _set_sel(cidx = @lbox.index('activate')) unless cidx
    cidx += 1
    if cidx == @lbox.index('end') - 1
      @down_arrow.pack_forget
    elsif cidx == @lbox.nearest(10000)
      @lbox.see(cidx + 1)
    end
  end

  def _config_proc
    if @lbox.size == 0
      @up_arrow.pack_forget
      @down_arrow.pack_forget
      return
    end
    tidx = @lbox.nearest(0)
    bidx = @lbox.nearest(10000)
    if tidx > 0
      _show_up_arrow
      tidx += 1
    else
      @up_arrow.pack_forget unless @up_timer.running?
    end
    if bidx < @lbox.index('end') - 1
      _show_down_arrow
      bidx -= 1
    else
      @down_arrow.pack_forget unless @down_timer.running?
    end
    cidx = @lbox.curselection[0]
    _check_sel(cidx, tidx, bidx) if cidx
  end

  def _init_binding
    @up_timer = TkAfter.new(@interval, -1, proc{_up_proc})
    @down_timer = TkAfter.new(@interval, -1, proc{_down_proc})

    @up_timer.set_start_proc(@initwait, proc{})
    @down_timer.set_start_proc(@initwait, proc{})

    @up_arrow.bind('Enter', proc{@up_timer.start})
    @up_arrow.bind('Leave', proc{@up_timer.stop if @up_arrow.winfo_mapped?})
    @down_arrow.bind('Enter', proc{@down_timer.start})
    @down_arrow.bind('Leave', proc{@down_timer.stop if @down_arrow.winfo_mapped?})

    @lbox.bind('Configure', proc{_config_proc})
    @lbox.bind('Enter', proc{|y| _set_sel(@lbox.nearest(y))}, '%y')
    @lbox.bind('Motion', proc{|y|
                 @up_timer.stop if @up_timer.running?
                 @down_timer.stop if @down_timer.running?
                 _check_sel(@lbox.nearest(y))
               }, '%y')

    @lbox.bind('Up', proc{_key_UP_proc})
    @lbox.bind('Down', proc{_key_DOWN_proc})
  end

  ############################
  public
  ############################
  def scrollbar(mode)
    if mode
      @scr.pack(:side=>:right, :fill=>:y)
    else
      @scr.pack_forget
    end
  end
end

################################################

class Tk::RbWidget::Combobox < TkEntry
  include TkComposite

  @@down_btn_bmp = TkBitmapImage.new(:data=><<EOD)
#define down_arrow_width 11
#define down_arrow_height 11
static unsigned char down_arrow_bits[] = {
   0x00, 0x00, 0xfe, 0x03, 0xfc, 0x01, 0xfc, 0x01, 0xf8, 0x00, 0xf8, 0x00,
   0x70, 0x00, 0x70, 0x00, 0x20, 0x00, 0x20, 0x00, 0x00, 0x00};
EOD

  @@up_btn_bmp = TkBitmapImage.new(:data=><<EOD)
#define up_arrow_width 11
#define up_arrow_height 11
static unsigned char up_arrow_bits[] = {
   0x00, 0x00, 0x20, 0x00, 0x20, 0x00, 0x70, 0x00, 0x70, 0x00, 0xf8, 0x00,
   0xf8, 0x00, 0xfc, 0x01, 0xfc, 0x01, 0xfe, 0x03, 0x00, 0x00};
EOD

  def _button_proc(dir = true)
    return if @ent.state == 'disabled'
    @btn.relief(:sunken)
    x = @frame.winfo_rootx
    y = @frame.winfo_rooty
    if dir
      @top.geometry("+#{x}+#{y + @frame.winfo_height}")
    else
      @btn.image(@@up_btn_bmp)
      @top.geometry("+#{x}+#{y - @top.winfo_reqheight}")
    end
    @top.deiconify
    @lst.focus

    if (idx = values.index(@ent.value))
      @lst.see(idx - 1)
      @lst.activate(idx)
      @lst.selection_set(idx)
    elsif @lst.size > 0
      @lst.see(0)
      @lst.activate(0)
      @lst.selection_set(0)
    end
    @top.grab

    begin
      @wait_var.tkwait
      if (idx = @wait_var.to_i) >= 0
        # @ent.value = @lst.get(idx)
        _set_entry_value(@lst.get(idx))
      end
      @top.withdraw
      @btn.relief(:raised)
      @btn.image(@@down_btn_bmp)
    rescue
    ensure
      begin
        @top.grab(:release)
        @ent.focus
      rescue
      end
    end
  end
  private :_button_proc

  def _init_bindings
    @btn.bind('1', proc{_button_proc(true)})
    @btn.bind('3', proc{_button_proc(false)})

    @lst.bind('1', proc{|y| @wait_var.value = @lst.nearest(y)}, '%y')
    @lst.bind('Return', proc{@wait_var.value = @lst.curselection[0]})

    cancel = TkVirtualEvent.new('2', '3', 'Escape')
    @lst.bind(cancel, proc{@wait_var.value = -1})
  end
  private :_init_bindings

  def _set_entry_value(val)
    @ent.textvariable.value = val
  end
  private :_set_entry_value

  #----------------------------------------------------

  def _state_control(value = None)
    if value == None
      # get
      @ent.state
    else
      # set
      @ent.state(value.to_s)
      case value = @ent.state # regulate 'state' string
      when 'normal', 'readonly'
        @btn.state 'normal'
      when 'disabled'
        @btn.state 'disabled'
      else
        # unknown : do nothing
      end
    end
  end
  private :_state_control

  def __methodcall_optkeys  # { key=>method, ... }
    {'state' => :_state_control}
  end
  private :__methodcall_optkeys

  #----------------------------------------------------

  def _textvariable_control(var = None)
    if var == None
      # get
      ((var = @ent.textvariable) === @default_var)? nil: var
    else
      # set
      @var = var
      tk_send('configure', '-textvariable', (@var)? var: @default_var)
    end
  end
  private :_textvariable_control

  #----------------------------------------------------

  def initialize_composite(keys={})
    keys = _symbolkey2str(keys)

    @btn = TkLabel.new(@frame, :relief=>:raised, :borderwidth=>2,
                       :image=>@@down_btn_bmp).pack(:side=>:right,
                                                    :ipadx=>2, :fill=>:y)
    @ent = TkEntry.new(@frame).pack(:side=>:left)
    @path = @ent.path

    @top = TkToplevel.new(@btn, :borderwidth=>1, :relief=>:raised) {
      withdraw
      transient
      overrideredirect(true)
    }

    startwait = keys.delete('startwait'){300}
    interval = keys.delete('interval'){150}
    @lst = Tk::RbWidget::AutoScrollListbox.new(@top, :scrollbar=>true,
                                               :startwait=>startwait,
                                               :interval=>interval)
    @lst.pack(:fill=>:both, :expand=>true)
    @ent_list = []

    @wait_var = TkVariable.new
    @var = @default_var = TkVariable.new

    @ent.textvariable @default_var

    _init_bindings

    option_methods('textvariable' => :_textvariable_control)

    delegate('DEFAULT', @ent)
    delegate('height', @lst)
    delegate('relief', @frame)
    delegate('borderwidth', @frame)

    delegate('arrowrelief', @lst)
    delegate('arrowborderwidth', @lst)

    delegate('state', false)

    if mode = keys.delete('scrollbar')
      scrollbar(mode)
    end

    configure keys unless keys.empty?
  end
  private :initialize_composite

  def scrollbar(mode)
    @lst.scrollbar(mode)
  end

  def _reset_width
    len = @ent.width
    @lst.get(0, 'end').each{|l| len = l.length if l.length > len}
    @lst.width(len + 1)
  end
  private :_reset_width

  def add(ent)
    ent = ent.to_s
    unless @ent_list.index(ent)
      @ent_list << ent
      @lst.insert('end', ent)
    end
    _reset_width
    self
  end

  def remove(ent)
    ent = ent.to_s
    @ent_list.delete(ent)
    if idx = @lst.get(0, 'end').index(ent)
      @lst.delete(idx)
    end
    _reset_width
    self
  end

  def values(ary = nil)
    if ary
      @lst.delete(0, 'end')
      @ent_list.clear
      ary.each{|ent| add(ent)}
      _reset_width
      self
    else
      @lst.get(0, 'end')
    end
  end

  def see(idx)
    @lst.see(@lst.index(idx) - 1)
  end

  def list_index(idx)
    @lst.index(idx)
  end
end


################################################
# test
################################################
if __FILE__ == $0
# e0 = Tk::RbWidget::Combobox.new.pack
# e0.values(%w(aa bb cc dd ee ff gg hh ii jj kk ll mm nn oo pp qq rr ss tt uu))

  v = TkVariable.new
  e = Tk::RbWidget::Combobox.new(:height=>7, :scrollbar=>true,
                                 :textvariable=>v,
                                 :arrowrelief=>:flat, :arrowborderwidth=>0,
                                 :startwait=>400, :interval=>200).pack
  e.values(%w(aa bb cc dd ee ff gg hh ii jj kk ll mm nn oo pp qq rr ss tt uu))
  #e.see(e.list_index('end') - 2)
  e.value = 'cc'
  TkFrame.new{|f|
    fnt = TkFont.new('Helvetica 10')
    TkLabel.new(f, :font=>fnt, :text=>'TkCombobox value :').pack(:side=>:left)
    TkLabel.new(f, :font=>fnt, :textvariable=>v).pack(:side=>:left)
  }.pack

  TkFrame.new(:relief=>:raised, :borderwidth=>2,
              :height=>3).pack(:fill=>:x, :expand=>true, :padx=>5, :pady=>3)

  l = Tk::RbWidget::AutoScrollListbox.new(nil, :relief=>:groove,
                                          :borderwidth=>4,:height=>7,
                                          :width=>20).pack(:fill=>:both,
                                                           :expand=>true)
  (0..20).each{|i| l.insert('end', "line #{i}")}

  TkFrame.new(:relief=>:ridge, :borderwidth=>3){
    TkButton.new(self, :text=>'ON',
                 :command=>proc{l.scrollbar(true)}).pack(:side=>:left)
    TkButton.new(self, :text=>'OFF',
                 :command=>proc{l.scrollbar(false)}).pack(:side=>:right)
    pack(:fill=>:x)
  }
  Tk.mainloop
end
