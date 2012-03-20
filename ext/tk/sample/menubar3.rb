#
# menubar sample 3 : vertical layout menubar; use frame and menubuttons
#

require 'tk'

radio_var = TkVariable.new('y')

menu_spec = [
  [['&File', true], # when underline option is ture, '&' index is the position
    {:label=>'Open', :command=>proc{puts('Open clicked')}, :underline=>0},
    '---',
    ['Check_A', TkVariable.new(true), 6],
    {:type=>'checkbutton', :label=>'Check_B',
                :variable=>TkVariable.new, :underline=>6},
    '---',
    ['Radio_X', [radio_var, 'x'], /[XYZ]/, '', {:foreground=>'black'}],
    ['Radio_Y', [radio_var, 'y'], /[XYZ]/],
    ['Radio_Z', [radio_var, 'z'], /[XYZ]/], # use Regexp for underline position
    '---',
    ['cascade', [
                   ['sss', proc{p 'sss'}, 0],
                   ['ttt', proc{p 'ttt'}, 0],
                   ['uuu', proc{p 'uuu'}, 0],
                   ['vvv', proc{p 'vvv'}, 0],
                ],
      0, '',
      {:font=>'Courier 16 italic',
       :menu_config=>{:font=>'Times -18 bold', :foreground=>'black'}}],
    '---',
    ['Quit', proc{exit}, 0]],

  [['Edit', 0],
    ['Cut', proc{puts('Cut clicked')}, 2],
    ['Copy', proc{puts('Copy clicked')}, 0],
    ['Paste', proc{puts('Paste clicked')}, 0]],

  [['Help', 0, {:menu_name=>'help'}],
    ['About This', proc{puts('Ruby/Tk menubar sample 3')}, "This"]]
                                      # use string index for underline position
]

layout_proc = 'vertical'
# The following procedure is same to 'layout_proc'=>'vertical'
=begin
layout_proc = proc{|parent, mbtn|
  mbtn.direction :right
  mbtn.pack(:side=>:top, :fill=>:x)

  menu = mbtn.menu
  cmd = proc{|m, dir|
    Tk::Menu::TkInternalFunction.next_menu(m, dir) rescue nil
    # ignore error when the internal function doesn't exist
  }
  menu.bind('Tab', cmd, :widget, 'forward')
  menu.bind('Alt-Tab', cmd, :widget, 'backward')
}
=end

menubar = TkMenubar.new(nil, menu_spec,
                        'layout_proc'=>layout_proc,
                        'tearoff'=>false,
                        'foreground'=>'grey40',
                        'activeforeground'=>'red',
                        'font'=>'Helvetia 12 bold')
menubar.pack('side'=>'left', 'fill'=>'y')

TkText.new(:wrap=>'word').pack.insert('1.0', 'This sample script generates "Menu Sidebar".
If "::tk::MenuNextMenuon" function is available  your Tcl/Tk library, you will be able to move to the next menu by Tab key on the posted menu, or the previous menu by Alt + Tab key.
Please read the sample source, and check how to override default configure options of menu entries on a menu_spec. Maybe, on windows, this menubar does not work properly about keyboard shortcuts. Then, please use "menu" option of root/toplevel widget (see sample/menubar3.rb).')

Tk.mainloop
