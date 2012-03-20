#
#  tktextframe.rb : a sample of TkComposite
#
#                         by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'

module Tk::ScrollbarComposite
  include TkComposite

  def component_construct_keys
    # If a component requires options for construction,
    # return an Array of option-keys.
    []
  end
  private :component_construct_keys

  def create_component(keys={})
    # This method must return the created component widget.
  end
  private :create_component

  def component_delegates
    # if want to override defalut option-methods or delegates,
    # please define here.
  end
  private :component_delegates

  def define_delegates
    # option methods for scrollbars
    option_methods([:scrollbarwidth, :get_scrollbarwidth])

    # set receiver widgets for configure methods (with alias)
    delegate_alias('scrollbarrelief', 'relief', @h_scroll, @v_scroll)
    delegate_alias('framebackground', 'background',
                   @frame, @h_scroll, @v_scroll)
    delegate_alias('activeframebackground', 'activebackground',
                   @h_scroll, @v_scroll)

    # set receiver widgets for configure methods
    delegate('DEFAULT', @component)
    delegate('troughcolor', @h_scroll, @v_scroll)
    delegate('repeatdelay', @h_scroll, @v_scroll)
    delegate('repeatinterval', @h_scroll, @v_scroll)
    delegate('borderwidth', @frame)
    delegate('relief', @frame)

    component_delegates
  end
  private :define_delegates

  DEFAULT_VSCROLL = true
  DEFAULT_HSCROLL = true

  def initialize_composite(keys={})
    keys = _symbolkey2str(keys)

    # create scrollbars
    @v_scroll = TkScrollbar.new(@frame, 'orient'=>'vertical')
    @h_scroll = TkScrollbar.new(@frame, 'orient'=>'horizontal')

    # create a component
    construct_keys = {}
    ((component_construct_keys.map{|k| k.to_s}) & keys.keys).each{|k|
      construct_keys[k] = keys.delete(k)
    }

    # create a component (the component must be scrollable)
    @component = create_component(construct_keys)

    # set default receiver of method calls
    @path = @component.path

    # assign scrollbars
    @component.xscrollbar(@h_scroll)
    @component.yscrollbar(@v_scroll)

    # allignment
    TkGrid.rowconfigure(@frame, 0, 'weight'=>1, 'minsize'=>0)
    TkGrid.columnconfigure(@frame, 0, 'weight'=>1, 'minsize'=>0)
    @component.grid('row'=>0, 'column'=>0, 'sticky'=>'news')

    # scrollbars ON
    vscroll(keys.delete('vscroll'){self.class::DEFAULT_VSCROLL})
    hscroll(keys.delete('hscroll'){self.class::DEFAULT_HSCROLL})

    # do configure
    define_delegates

    # do configure
    configure keys unless keys.empty?
  end
  private :initialize_composite

  # get/set width of scrollbar
  def get_scrollbarwidth
    @v_scroll.width
  end
  def set_scrollbarwidth(width)
    @v_scroll.width(width)
    @h_scroll.width(width)
  end
  alias :scrollbarwidth :set_scrollbarwidth

  def hook_vscroll_on(*args); end
  def hook_vscroll_off(*args); end
  def hook_hscroll_on(*args); end
  def hook_hscroll_off(*args); end
  private :hook_vscroll_on,:hook_vscroll_off,:hook_hscroll_on,:hook_hscroll_off

  # vertical scrollbar : ON/OFF
  def vscroll(mode, *args)
    st = TkGrid.info(@v_scroll)
    if mode && st.size == 0 then
      @v_scroll.grid('row'=>0, 'column'=>1, 'sticky'=>'ns')
      hook_vscroll_on(*args)
    elsif !mode && st.size != 0 then
      @v_scroll.ungrid
      hook_vscroll_off(*args)
    end
    self
  end

  # horizontal scrollbar : ON/OFF
  def hscroll(mode, *args)
    st = TkGrid.info(@h_scroll)
    if mode && st.size == 0 then
      @h_scroll.grid('row'=>1, 'column'=>0, 'sticky'=>'ew')
      hook_hscroll_on(*args)
    elsif !mode && st.size != 0 then
      @h_scroll.ungrid
      hook_hscroll_off(*args)
    end
    self
  end
end

################################################

class TkTextFrame < TkText
  include Tk::ScrollbarComposite

  # def component_construct_keys; []; end
  # private :component_construct_keys

  def create_component(keys={})
    # keys has options which are listed by component_construct_keys method.
    @text = TkText.new(@frame, 'wrap'=>'none')
    @text.configure(keys) unless keys.empty?

    # option methods for component
    option_methods(
       [:textbackground, nil, :textbg_info],
       :textborderwidth,
       :textrelief
    )

    # return the created componet
    @text
  end
  private :create_component

  # def component_delegates; end
  # private :component_delegates

  def hook_hscroll_on(wrap_mode=nil)
    if wrap_mode
      wrap wrap_mode
    else
      wrap 'none'  # => self.wrap('none')
    end
  end
  def hook_hscroll_off(wrap_mode)
    wrap wrap_mode  # => self.wrap(wrap_mode)
  end
  def hscroll(mode, wrap_mode="char")
    super
  end

  # set background color of text widget
  def textbackground(color = nil)
    if color
      @text.background(color)
    else
      @text.background
    end
  end

  def textbg_info
    info = @text.configinfo(:background)
    if TkComm::GET_CONFIGINFO_AS_ARRAY
      info[0] = 'textbackground'
      info
    else # ! TkComm::GET_CONFIGINFO_AS_ARRAY
      {'textbackground' => info['background']}
    end
  end

  # get/set borderwidth of text widget
  def set_textborderwidth(width)
    @text.borderwidth(width)
  end
  def get_textborderwidth
    @text.borderwidth
  end
  def textborderwidth(width = nil)
    if width
      set_textborderwidth(width)
    else
      get_textborderwidth
    end
  end

  # set relief of text widget
  def textrelief(type)
    @text.relief(type)
  end
end

################################################
# test
################################################
if __FILE__ == $0
  TkLabel.new(:text=>'TkTextFrame is an example of Tk::ScrollbarComposite module.').pack
  f = TkFrame.new.pack('fill'=>'x')
  #t = TkTextFrame.new.pack
  t = TkTextFrame.new(:textborderwidth=>3,
                      :textrelief=>:ridge,
                      :scrollbarrelief=>:ridge).pack
  p t.configinfo
  TkButton.new(f, 'text'=>'vscr OFF',
               'command'=>proc{t.vscroll(false)}).pack('side'=>'right')
  TkButton.new(f, 'text'=>'vscr ON',
               'command'=>proc{t.vscroll(true)}).pack('side'=>'right')
  TkButton.new(f, 'text'=>'hscr ON',
               'command'=>proc{t.hscroll(true)}).pack('side'=>'left')
  TkButton.new(f, 'text'=>'hscr OFF',
               'command'=>proc{t.hscroll(false)}).pack('side'=>'left')

  ############################################

  # Tk.default_widget_set = :Ttk

  TkFrame.new.pack(:pady=>10)
  TkLabel.new(:text=>'The following is another example of Tk::ScrollbarComposite module.').pack

  #----------------------------------
  class ScrListbox < TkListbox
    include Tk::ScrollbarComposite

    DEFAULT_HSCROLL = false

    def create_component(keys={})
      TkListbox.new(@frame, keys)
    end
    private :create_component
  end
  #----------------------------------

  f = TkFrame.new.pack(:pady=>5)
  lbox = ScrListbox.new(f).pack(:side=>:left)
  lbox.value = %w(aa bb cc dd eeeeeeeeeeeeeeeeeeeeeeeeee ffffffffff gg hh ii jj kk ll mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm nn oo pp qq)
  fb = TkFrame.new(f).pack(:expand=>true, :fill=>:y, :padx=>5)
  TkButton.new(fb, 'text'=>'lbox hscr OFF',
               'command'=>proc{lbox.hscroll(false)}).pack(:side=>:bottom,
                                                          :fill=>:x)
  TkButton.new(fb, 'text'=>'lbox hscr ON',
               'command'=>proc{lbox.hscroll(true)}).pack(:side=>:bottom,
                                                          :fill=>:x)
  TkFrame.new(fb).pack(:pady=>5, :side=>:bottom)
  TkButton.new(fb, 'text'=>'lbox vscr OFF',
               'command'=>proc{lbox.vscroll(false)}).pack(:side=>:bottom,
                                                          :fill=>:x)
  TkButton.new(fb, 'text'=>'lbox vscr ON',
               'command'=>proc{lbox.vscroll(true)}).pack(:side=>:bottom,
                                                          :fill=>:x)

  ############################################

  Tk.mainloop
end
