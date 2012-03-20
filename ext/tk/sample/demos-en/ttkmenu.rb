# ttkmenu.rb --
#
# This demonstration script creates a toplevel window containing several Ttk
# menubutton widgets.
#
# based on "Id: ttkmenu.tcl,v 1.3 2007/12/13 15:27:07 dgp Exp"

if defined?($ttkmenu_demo) && $ttkmenu_demo
  $ttkmenu_demo.destroy
  $ttkmenu_demo = nil
end

$ttkmenu_demo = TkToplevel.new {|w|
  title("Ttk Menu Buttons")
  iconname("ttkmenu")
  positionWindow(w)
}

base_frame = Ttk::Frame.new($ttkmenu_demo).pack(:fill=>:both, :expand=>true)

Ttk::Label.new(base_frame, :font=>$font, :wraplength=>'4i', :justify=>:left,
               :text=><<EOL).pack(:side=>:top, :fill=>:x)
Ttk is the new Tk themed widget set, \
and one widget that is available in themed form is the menubutton. \
Below are some themed menu buttons \
that allow you to pick the current theme in use. \
Notice how picking a theme changes the way \
that the menu buttons themselves look, \
and that the central menu button is styled differently \
(in a way that is normally suitable for toolbars). \
However, there are no themed menus; the standard Tk menus were judged \
to have a sufficiently good look-and-feel on all platforms, \
especially as they are implemented as native controls in many places.
EOL

Ttk::Separator.new(base_frame).pack(:side=>:top, :fill=>:x)

## See Code / Dismiss
Ttk::Frame.new($ttkmenu_demo) {|frame|
  sep = Ttk::Separator.new(frame)
  Tk.grid(sep, :columnspan=>4, :row=>0, :sticky=>'ew', :pady=>2)
  TkGrid('x',
         Ttk::Button.new(frame, :text=>'See Code',
                         :image=>$image['view'], :compound=>:left,
                         :command=>proc{showCode 'ttkmenu'}),
         Ttk::Button.new(frame, :text=>'Dismiss',
                         :image=>$image['delete'], :compound=>:left,
                         :command=>proc{
                           $ttkmenu_demo.destroy
                           $ttkmenu_demo = nil
                         }),
         :padx=>4, :pady=>4)
  grid_columnconfigure(0, :weight=>1)
  pack(:side=>:bottom, :fill=>:x)
}

b1 = Ttk::Menubutton.new(base_frame,:text=>'Select a theme',:direction=>:above)
b2 = Ttk::Menubutton.new(base_frame,:text=>'Select a theme',:direction=>:left)
b3 = Ttk::Menubutton.new(base_frame,:text=>'Select a theme',:direction=>:right)
b4 = Ttk::Menubutton.new(base_frame,:text=>'Select a theme',:direction=>:flush,
                         :style=>Ttk::Menubutton.style('Toolbutton'))
b5 = Ttk::Menubutton.new(base_frame,:text=>'Select a theme',:direction=>:below)

b1.menu(m1 = Tk::Menu.new(b1, :tearoff=>false))
b2.menu(m2 = Tk::Menu.new(b2, :tearoff=>false))
b3.menu(m3 = Tk::Menu.new(b3, :tearoff=>false))
b4.menu(m4 = Tk::Menu.new(b4, :tearoff=>false))
b5.menu(m5 = Tk::Menu.new(b5, :tearoff=>false))

Ttk.themes.each{|theme|
  m1.add(:command, :label=>theme, :command=>proc{Ttk.set_theme theme})
  m2.add(:command, :label=>theme, :command=>proc{Ttk.set_theme theme})
  m3.add(:command, :label=>theme, :command=>proc{Ttk.set_theme theme})
  m4.add(:command, :label=>theme, :command=>proc{Ttk.set_theme theme})
  m5.add(:command, :label=>theme, :command=>proc{Ttk.set_theme theme})
}

f = Ttk::Frame.new(base_frame).pack(:fill=>:x)
f1 = Ttk::Frame.new(base_frame).pack(:fill=>:both, :expand=>true)
f.lower

f.grid_anchor(:center)
TkGrid('x', b1, 'x', :in=>f, :padx=>3, :pady=>2)
TkGrid(b2,  b4, b3,  :in=>f, :padx=>3, :pady=>2)
TkGrid('x', b5, 'x', :in=>f, :padx=>3, :pady=>2)
