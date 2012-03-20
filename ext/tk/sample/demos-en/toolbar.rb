# toolbar.rb --
#
# This demonstration script creates a toolbar that can be torn off.
#
# based on "Id: toolbar.tcl,v 1.3 2007/12/13 15:27:07 dgp Exp"

if defined?($toolbar_demo) && $toolbar_demo
  $toolbar_demo.destroy
  $toolbar_demo = nil
end

$toolbar_demo = TkToplevel.new {|w|
  title("Ttk Menu Buttons")
  iconname("toolbar")
  positionWindow(w)
}

base_frame = Ttk::Frame.new($toolbar_demo).pack(:fill=>:both, :expand=>true)

if Tk.windowingsystem != 'aqua'
  msg = Ttk::Label.new(base_frame, :wraplength=>'4i',
                       :text=>Tk::UTF8_String.new(<<EOL))
This is a demonstration of how to do \
a toolbar that is styled correctly \
and which can be torn off (this feature reqrires Tcl/Tk8.5). \
The buttons are configured to be \\u201Ctoolbar style\\u201D buttons by \
telling them that they are to use the Toolbutton style. At the left \
end of the toolbar is a simple marker that the cursor changes to a \
movement icon over; drag that away from the toolbar to tear off the \
whole toolbar into a separate toplevel widget. When the dragged-off \
toolbar is no longer needed, just close it like any normal toplevel \
and it will reattach to the window it was torn off from.
EOL
else
  msg = Ttk::Label.new(base_frame, :wraplength=>'4i',
                       :text=>Tk::UTF8_String.new(<<EOL))
This is a demonstration of how to do \
a toolbar that is styled correctly. The buttons are configured to \
be \\u201Ctoolbar style\\u201D buttons by telling them that they are \
to use the Toolbutton style.
EOL
end

## Set up the toolbar hull
tbar_base = Tk::Frame.new(base_frame, # Must be a starndard Tk frame!
                          :widgetname=>'toolbar') # for window title
sep = Ttk::Separator.new(base_frame)
to_base = Ttk::Frame.new(tbar_base, :cursor=>'fleur')
if Tk.windowingsystem != 'aqua'
  to  = Ttk::Separator.new(to_base, :orient=>:vertical)
  to2 = Ttk::Separator.new(to_base, :orient=>:vertical)
  to.pack(:fill=>:y, :expand=>true, :padx=>2, :side=>:left)
  to2.pack(:fill=>:y, :expand=>true, :side=>:left)
end

contents = Ttk::Frame.new(tbar_base)
Tk.grid(to_base, contents, :sticky=>'nsew')
tbar_base.grid_columnconfigure(contents, :weight=>1)
contents.grid_columnconfigure(1000, :weight=>1)

if Tk.windowingsystem != 'aqua'
  ## Bindings so that the toolbar can be torn off and reattached
  to_base.bind('B1-Motion', '%X %Y'){|x, y| tbar_base.tearoff(to_base, x, y)}
  to.     bind('B1-Motion', '%X %Y'){|x, y| tbar_base.tearoff(to_base, x, y)}
  to2.    bind('B1-Motion', '%X %Y'){|x, y| tbar_base.tearoff(to_base, x, y)}
  def tbar_base.tearoff(w, x, y)
    on_win = TkWinfo.containing(x, y)
    return unless (on_win && on_win.path =~ /^#{@path}(\.|$)/)
    self.grid_remove
    w.grid_remove
    self.wm_manage
    # self.wm_title('Toolbar') # if you don't want to use its widget name as a window title.
    self.wm_protocol('WM_DELETE_WINDOW'){ self.untearoff(w) }
  end
  def tbar_base.untearoff(w)
    self.wm_forget
    w.grid
    self.grid
  end
end

## Some content for the rest of the toplevel
text = TkText.new(base_frame, :width=>40, :height=>10)

## Toolbar contents
tb_btn = Ttk::Button.new(tbar_base, :text=>'Button', :style=>'Toolbutton',
                         :command=>proc{text.insert(:end, "Button Pressed\n")})
tb_chk = Ttk::Checkbutton.new(tbar_base, :text=>'Check', :style=>'Toolbutton',
                              :variable=>(check = TkVariable.new),
                              :command=>proc{
                                text.insert(:end, "Check is #{check.value}\n")
                              })
tb_mbtn = Ttk::Menubutton.new(tbar_base, :text=>'Menu')
tb_combo = Ttk::Combobox.new(tbar_base, :value=>TkFont.families,
                             :state=>:readonly)
tb_mbtn.menu(menu = Tk::Menu.new(tb_mbtn))
menu.add(:command, :label=>'Just', :command=>proc{text.insert(:end, "Just\n")})
menu.add(:command, :label=>'An', :command=>proc{text.insert(:end, "An\n")})
menu.add(:command, :label=>'Example',
         :command=>proc{text.insert(:end, "Example\n")})
tb_combo.bind('<ComboboxSelected>'){ text.font.family = tb_combo.get }

## Arrange contents
Tk.grid(tb_btn, tb_chk, tb_mbtn, tb_combo,
        :in=>contents, :padx=>2, :sticky=>'ns')
Tk.grid(tbar_base, :sticky=>'ew')
Tk.grid(sep, :sticky=>'ew')
Tk.grid(msg, :sticky=>'ew')
Tk.grid(text, :sticky=>'nsew')
base_frame.grid_rowconfigure(text, :weight=>1)
base_frame.grid_columnconfigure(text, :weight=>1)

## See Code / Dismiss buttons
Ttk::Frame.new(base_frame) {|frame|
  sep = Ttk::Separator.new(frame)
  Tk.grid(sep, :columnspan=>4, :row=>0, :sticky=>'ew', :pady=>2)
  TkGrid('x',
         Ttk::Button.new(frame, :text=>'See Code',
                         :image=>$image['view'], :compound=>:left,
                         :command=>proc{showCode 'toolbar'}),
         Ttk::Button.new(frame, :text=>'Dismiss',
                         :image=>$image['delete'], :compound=>:left,
                         :command=>proc{
                           $toolbar_demo.destroy
                           $toolbar_demo = nil
                         }),
         :padx=>4, :pady=>4)
  grid_columnconfigure(0, :weight=>1)
  Tk.grid(frame, :sticky=>'ew')
}
