#
#  Tk::RbWidget::ScrollFrame class
#
#    This widget class is a frame widget with scrollbars.
#    The ScrollFrame doesn't propagate the size of embedded widgets.
#    When it is configured, scrollregion of the container is changed.
#
#    Scrollbars can be toggled by Tk::RbWidget::ScrollFrame#vscroll & hscroll.
#    If horizontal or virtical scrollbar is turned off, the horizontal
#    or virtical size of embedded widgets is propagated.
#
#                         Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'

class Tk::RbWidget::ScrollFrame < TkFrame
  include TkComposite

  DEFAULT_WIDTH  = 200
  DEFAULT_HEIGHT = 200

  def initialize_composite(keys={})
    @frame.configure(:width=>DEFAULT_WIDTH, :height=>DEFAULT_HEIGHT)

    # create scrollbars
    @h_scroll = TkScrollbar.new(@frame, 'orient'=>'horizontal')
    @v_scroll = TkScrollbar.new(@frame, 'orient'=>'vertical')

    # create a canvas widget
    @canvas = TkCanvas.new(@frame,
                           :borderwidth=>0, :selectborderwidth=>0,
                           :highlightthickness=>0)

    # allignment
    TkGrid.rowconfigure(@frame, 0, 'weight'=>1, 'minsize'=>0)
    TkGrid.columnconfigure(@frame, 0, 'weight'=>1, 'minsize'=>0)
    @canvas.grid('row'=>0, 'column'=>0, 'sticky'=>'news')
    @frame.grid_propagate(false)

    # assign scrollbars
    @canvas.xscrollbar(@h_scroll)
    @canvas.yscrollbar(@v_scroll)

    # convert hash keys
    keys = _symbolkey2str(keys)

    # check options for the frame
    framekeys = {}
    if keys.key?('classname')
       keys['class'] = keys.delete('classname')
    end
    if @classname = keys.delete('class')
      framekeys['class'] = @classname
    end
    if @colormap  = keys.delete('colormap')
      framekeys['colormap'] = @colormap
    end
    if @container = keys.delete('container')
      framekeys['container'] = @container
    end
    if @visual    = keys.delete('visual')
      framekeys['visual'] = @visual
    end
    if @classname.kind_of? TkBindTag
      @db_class = @classname
      @classname = @classname.id
    elsif @classname
      @db_class = TkDatabaseClass.new(@classname)
    else
      @db_class = self.class
      @classname = @db_class::WidgetClassName
    end

    # create base frame
    @base = TkFrame.new(@canvas, framekeys)

    # embed base frame
    @cwin = TkcWindow.new(@canvas, [0, 0], :window=>@base, :anchor=>'nw')
    @canvas.scrollregion(@cwin.bbox)

    # binding to reset scrollregion
    @base.bind('Configure'){ _reset_scrollregion(nil, nil) }

    # set default receiver of method calls
    @path = @base.path

    # scrollbars ON
    vscroll(keys.delete('vscroll'){true})
    hscroll(keys.delete('hscroll'){true})

    # please check the differences of the following definitions
    option_methods(
      :scrollbarwidth
    )

    # set receiver widgets for configure methods (with alias)
    delegate_alias('scrollbarrelief', 'relief', @h_scroll, @v_scroll)

    # set receiver widgets for configure methods
    delegate('DEFAULT', @base)
    delegate('background', @frame, @base, @canvas, @h_scroll, @v_scroll)
    delegate('width', @frame)
    delegate('height', @frame)
    delegate('activebackground', @h_scroll, @v_scroll)
    delegate('troughcolor', @h_scroll, @v_scroll)
    delegate('repeatdelay', @h_scroll, @v_scroll)
    delegate('repeatinterval', @h_scroll, @v_scroll)
    delegate('borderwidth', @frame)
    delegate('relief', @frame)

    # do configure
    configure keys unless keys.empty?
  end

  # callback for Configure event
  def _reset_scrollregion(h_mod=nil, v_mod=nil)
    cx1, cy1, cx2, cy2 = @canvas.scrollregion
    x1, y1, x2, y2 = @cwin.bbox
    @canvas.scrollregion([x1, y1, x2, y2])

    if h_mod.nil? && v_mod.nil?
      if x2 != cx2 && TkGrid.info(@h_scroll).size == 0
        @frame.grid_propagate(true)
        @canvas.width  = x2
        Tk.update_idletasks
        @frame.grid_propagate(false)
      end
      if y2 != cy2 && TkGrid.info(@v_scroll).size == 0
        @frame.grid_propagate(true)
        @canvas.height = y2
        Tk.update_idletasks
        @frame.grid_propagate(false)
      end
    else
      @h_scroll.ungrid if h_mod == false
      @v_scroll.ungrid if v_mod == false

      h_flag = (TkGrid.info(@h_scroll).size == 0)
      v_flag = (TkGrid.info(@v_scroll).size == 0)

      @frame.grid_propagate(true)

      @canvas.width  = (h_flag)? x2: @canvas.winfo_width
      @canvas.height = (v_flag)? y2: @canvas.winfo_height

      @h_scroll.grid('row'=>1, 'column'=>0, 'sticky'=>'ew') if h_mod
      @v_scroll.grid('row'=>0, 'column'=>1, 'sticky'=>'ns') if v_mod

      Tk.update_idletasks

      @frame.grid_propagate(false)
    end
  end
  private :_reset_scrollregion

  # forbid to change binding of @base frame
  def bind(*args)
    @frame.bind(*args)
  end
  def bind_append(*args)
    @frame.bind_append(*args)
  end
  def bind_remove(*args)
    @frame.bind_remove(*args)
  end
  def bindinfo(*args)
    @frame.bindinfo(*args)
  end

  # set width of scrollbar
  def scrollbarwidth(width = nil)
    if width
      @h_scroll.width(width)
      @v_scroll.width(width)
    else
      @h_scroll.width
    end
  end

  # vertical scrollbar : ON/OFF
  def vscroll(mode)
    Tk.update_idletasks
    st = TkGrid.info(@v_scroll)
    if mode && st.size == 0 then
      @v_scroll.grid('row'=>0, 'column'=>1, 'sticky'=>'ns')
      _reset_scrollregion(nil, true)
    elsif !mode && st.size != 0 then
      _reset_scrollregion(nil, false)
    else
      _reset_scrollregion(nil, nil)
    end
    self
  end

  # horizontal scrollbar : ON/OFF
  def hscroll(mode)
    Tk.update_idletasks
    st = TkGrid.info(@h_scroll)
    if mode && st.size == 0 then
      _reset_scrollregion(true, nil)
    elsif !mode && st.size != 0 then
      _reset_scrollregion(false, nil)
    else
      _reset_scrollregion(nil, nil)
    end
    self
  end
end

# test
if __FILE__ == $0
  f = Tk::RbWidget::ScrollFrame.new(:scrollbarwidth=>10,
                                    :width=>300, :height=>200)
  f.pack(:expand=>true, :fill=>:both)

  TkButton.new(f, :text=>'foo button', :command=>proc{puts 'foo'}).pack
  TkButton.new(f, :text=>'baaar button', :command=>proc{puts 'baaar'}).pack
  TkButton.new(f, :text=>'baz button', :command=>proc{puts 'baz'}).pack
  TkButton.new(f, :text=>'hoge hoge button',
               :command=>proc{puts 'hoge hoge'}).pack(:side=>:bottom)

  # f.hscroll(false)

  # add a text widget
  Tk.after(3000){
    t = TkText.new(f).pack(:expand=>true, :fill=>:both)
    t.insert(:end, "An example of Tk::RbWidget::ScrollFrame widget.\n\n")
    t.insert(:end, "Here is a text widget.\n")
    t.insert(:end, "Please resize the application window, ")
    t.insert(:end, "and try the scrollbars ")
    t.insert(:end, "to move the view of packed widgets.\n")
  }

  # remove a vertical scrollbar, and then the scrollframe is not scrollable.
  Tk.after(6000){ f.vscroll(false) }

  # add a vertical scrollbar, and make the scrollframe scrollable.
  Tk.after(9000){ f.vscroll(true) }

  # remove a horizontal scrollbar, and then the scrollframe is not scrollable.
  Tk.after(12000){ f.hscroll(false) }

  # add a horizontal scrollbar, and make the scrollframe scrollable.
  Tk.after(15000){ f.hscroll(true) }

  Tk.mainloop
end
