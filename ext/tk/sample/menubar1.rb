#
# menubar sample 1 : use frame and menubuttons
#

require 'tk'

radio_var = TkVariable.new('y')

menu_spec = [
  [['File', 0],
    {:label=>'Open', :command=>proc{puts('Open clicked')}, :underline=>0},
    '---',
    ['Check_A', TkVariable.new(true), 6],
    {:type=>'checkbutton', :label=>'Check_B',
                :variable=>TkVariable.new, :underline=>6},
    '---',
    ['Radio_X', [radio_var, 'x'], 6, '', {:foreground=>'black'}],
    ['Radio_Y', [radio_var, 'y'], 6],
    ['Radio_Z', [radio_var, 'z'], 6],
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
    ['About This', proc{puts('Ruby/Tk menubar sample 1')}, 6]]
]

menubar = TkMenubar.new(nil, menu_spec,
                       'tearoff'=>false,
                       'foreground'=>'grey40',
                       'activeforeground'=>'red',
                       'font'=>'Helvetia 12 bold')
menubar.pack('side'=>'top', 'fill'=>'x')

TkText.new(:wrap=>'word').pack.insert('1.0', 'Please read the sample source, and check how to override default configure options of menu entries on a menu_spec. Maybe, on windows, this menubar does not work properly about keyboard shortcuts. Then, please use "menu" option of root/toplevel widget (see sample/menubar2.rb).')

Tk.mainloop
