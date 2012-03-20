#
# menubar sample 2 : use 'menu' option of root/toplevel widget
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
    ['About This', proc{puts('Ruby/Tk menubar sample 2')}, 6]]
]

mbar = Tk.root.add_menubar(menu_spec,
                           # followings are default configure options
                           'tearoff'=>'false',
                           'foreground'=>'grey40',
                           'activeforeground'=>'red',
                           'font'=>'Helvetia 12 bold')
# This (default configure options) is NOT same the following.
#
#  mbar = Tk.root.add_menubar(menu_spec)
#  mbar.configure('foreground'=>'grey40', 'activeforeground'=>'red',
#                 'font'=>'Helvetia 12 bold')

TkText.new(:wrap=>'word').pack.insert('1.0', 'Please read the sample source, and check how to override default configure options of menu entries on a menu_spec.')

Tk.mainloop
