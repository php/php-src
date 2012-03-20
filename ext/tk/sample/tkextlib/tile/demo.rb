#!/usr/bin/env ruby
#
# Demo for 'tile' package.
#
require 'tk'

demodir = File.dirname($0)
themesdir = File.join(demodir, 'themes')
Tk::AUTO_PATH.lappend('.', demodir, themesdir)

Dir.foreach(themesdir){|name|
  next if name == '.' || name == '..'
  dir = File.join(themesdir, name)
  Tk::AUTO_PATH.lappend(dir) if File.directory?(dir)
}

require 'tkextlib/tile'

def version?(ver)
  TkPackage.vcompare(Tk::Tile.package_version, ver) >= 0
end

# define Tcl/Tk procedures for compatibility
Tk::Tile.__define_LoadImages_proc_for_compatibility__!
Tk::Tile::Style.__define_wrapper_proc_for_compatibility__!

unless Tk::Tile::Style.theme_names.include?('step')
  Tk::Tile::Style.theme_create('step')
end

Tk.load_tclscript(File.join(demodir, 'toolbutton.tcl'))
Tk.load_tclscript(File.join(demodir, 'repeater.tcl'))

# This forces an update of the available packages list. It's required
# for package names to find the themes in demos/themes/*.tcl
## Tk.tk_call(TkPackage.unknown_proc, 'Tcl', TkPackage.provide('Tcl'))
##  --> This doesn't work.
##      Because, unknown_proc may be "command + some arguments".
Tk.ip_eval("#{TkPackage.unknown_proc}  Tcl #{TkPackage.provide('Tcl')}")

TkRoot.new{
  title 'Tile demo'
  iconname 'Tile demo'
}

# The descriptive names of the builtin themes.
$THEMELIST = [
  ['default', 'Default'],
  ['classic', 'Classic'],
  ['alt', 'Revitalized'],
  ['winnative', 'Windows native'],
  ['xpnative', 'XP Native'],
  ['aqua', 'Aqua'],
]

$V = TkVariable.new_hash(:THEME      => 'default',
                         :COMPOUND   => 'top',
                         :CONSOLE    => false,
                         :MENURADIO1 => 'One',
                         :MENUCHECK1 => true,
                         :PBMODE     => 'determinate',
                         :SELECTED   => true,
                         :CHOICE     => 2)

# Add in any available loadable themes.
TkPackage.names.find_all{|n| n =~ /^(tile|ttk)::theme::/}.each{|pkg|
  name = pkg.split('::')[-1]
  unless $THEMELIST.assoc(name)
    $THEMELIST << [name, Tk.tk_call('string', 'totitle', name)]
  end
}

# Add theme definition written by ruby
$RUBY_THEMELIST = []
begin
  load(File.join(demodir, 'themes', 'kroc.rb'), true)
rescue => e
raise e
  $RUBY_THEMELIST << ['kroc-rb', 'Kroc (by Ruby)', false]
else
  $RUBY_THEMELIST << ['kroc-rb', 'Kroc (by Ruby)', true]
end

def makeThemeControl(parent)
  c = Tk::Tile::Labelframe.new(parent, :text=>'Theme')
  $THEMELIST.each{|theme, name|
    b = Tk::Tile::Radiobutton.new(c, :text=>name, :value=>theme,
                                  :variable=>$V.ref(:THEME),
                                  :command=>proc{setTheme(theme)})
    b.grid(:sticky=>:ew)
    unless (TkPackage.names.find{|n| n =~ /(tile|ttk)::theme::#{theme}/})
      b.ttk_state(:disabled)
    end
  }
  $RUBY_THEMELIST.each{|theme, name, available|
    b = Tk::Tile::Radiobutton.new(c, :text=>name, :value=>theme,
                                  :variable=>$V.ref(:THEME),
                                  :command=>proc{setTheme(theme)})
    b.grid(:sticky=>:ew)
    b.ttk_state(:disabled) unless available
  }
  c
end

def makeThemeMenu(parent)
  m = TkMenu.new(parent)
  $THEMELIST.each{|theme, name|
    m.add(:radiobutton, :label=>name, :variable=>$V.ref(:THEME),
          :value=>theme, :command=>proc{setTheme(theme)})
    unless (TkPackage.names.find{|n| n =~ /(tile|ttk)::theme::#{theme}/})
      m.entryconfigure(:end, :state=>:disabled)
    end
  }
  $RUBY_THEMELIST.each{|theme, name, available|
    m.add(:radiobutton, :label=>name, :variable=>$V.ref(:THEME),
          :value=>theme, :command=>proc{setTheme(theme)})
    m.entryconfigure(:end, :state=>:disabled) unless available
  }
  m
end

def setTheme(theme)
  if (pkg = TkPackage.names.find{|n| n =~ /(tile|ttk)::theme::#{theme}/})
    unless Tk::Tile::Style.theme_names.find{|n| n == theme}
      TkPackage.require(pkg)
    end
  end
  Tk::Tile::Style.theme_use(theme)
end

#
# Load icons...
#
$BUTTONS = ['open', 'new', 'save']
$CHECKBOXES = ['bold', 'italic']
$ICON = {}

def loadIcons(file)
  Tk.load_tclscript(file)
  img_data = TkVarAccess.new('ImgData')
  img_data.keys.each{|icon|
    $ICON[icon] = TkPhotoImage.new(:data=>img_data[icon])
  }
end

loadIcons(File.join(demodir, 'iconlib.tcl'))

#
# Utilities:
#
def foreachWidget(wins, cmd)
  wins.each{|w|
    cmd.call(w)
    foreachWidget(w.winfo_children, cmd)
  }
end

# sbstub
#	Used as the :command option for a scrollbar,
#	updates the scrollbar's position.
#
def sbstub(sb, cmd, num, units = 'units')
  num = TkComm.number(num)
  case cmd.to_s
  when 'moveto'
    sb.set(num, num+0.5)

  when 'scroll'
    if units.to_s == 'pages'
      delta = 0.2
    else
      delta = 0.05
    end
    current = sb.get
    sb.set(current[0] + delta * num, current[1] + delta * num)
  end
end

# ... for debugging:
TkBindTag::ALL.bind('ButtonPress-3', proc{|w| $W = w}, '%W')
TkBindTag::ALL.bind('Control-ButtonPress-3', proc{|w| w.set_focus}, '%W')

def showHelp()
  Tk.messageBox(:message=>'No help yet...')
end

#
# See toolbutton.tcl.
TkOption.add('*Toolbar.relief', :groove)
TkOption.add('*Toolbar.borderWidth', 2)

TkOption.add('*Toolbar.Button.Pad', 2)

$ROOT = Tk.root
$BASE = $ROOT
Tk.destroy(*($ROOT.winfo_children))

$TOOLBARS = []

#
# Toolbar button standard vs. tile comparison:
#
def makeToolbars
  #
  # Tile toolbar:
  #
  tb = Tk::Tile::Frame.new($BASE, :class=>'Toolbar')
  $TOOLBARS << tb
  i = 0
  $BUTTONS.each{|icon|
    i += 1
    Tk::Tile::Button.new(tb, :text=>icon, :image=>$ICON[icon],
                         :compound=>$V[:COMPOUND],
                         :style=>:Toolbutton).grid(:row=>0, :column=>i,
                                                   :sticky=>:news)
  }
  $CHECKBOXES.each{|icon|
    i += 1
    Tk::Tile::Checkbutton.new(tb, :text=>icon, :image=>$ICON[icon],
                              :variable=>$V.ref(icon),
                              :compound=>$V[:COMPOUND],
                              :style=>:Toolbutton).grid(:row=>0, :column=>i,
                                                        :sticky=>:news)
  }

  mb = Tk::Tile::Menubutton.new(tb, :text=>'toolbar', :image=>$ICON['file'],
                                :compound=>$V[:COMPOUND])
  mb.configure(:menu=>makeCompoundMenu(mb))
  i += 1
  mb.grid(:row=>0, :column=>i, :sticky=>:news)

  i += 1
  tb.grid_columnconfigure(i, :weight=>1)

  #
  # Standard toolbar:
  #
  tb = TkFrame.new($BASE, :class=>'Toolbar')
  $TOOLBARS << tb
  i = 0
  $BUTTONS.each{|icon|
    i += 1
    TkButton.new(tb, :text=>icon, :image=>$ICON[icon],
                 :compound=>$V[:COMPOUND], :relief=>:flat,
                 :overrelief=>:raised).grid(:row=>0, :column=>i,
                                            :sticky=>:news)
  }
  $CHECKBOXES.each{|icon|
    i += 1
    TkCheckbutton.new(tb, :text=>icon, :image=>$ICON[icon],
                      :variable=>$V.ref(icon), :compound=>$V[:COMPOUND],
                      :indicatoron=>false, :selectcolor=>'', :relief=>:flat,
                      :overrelief=>:raised).grid(:row=>0, :column=>i,
                                                 :sticky=>:news)
  }

  mb = TkMenubutton.new(tb, :text=>'toolbar', :image=>$ICON['file'],
                        :compound=>$V[:COMPOUND])
  mb.configure(:menu=>makeCompoundMenu(mb))
  i += 1
  mb.grid(:row=>0, :column=>i, :sticky=>:news)

  i += 1
  tb.grid_columnconfigure(i, :weight=>1)
end

#
# Toolbar :compound control:
#
def makeCompoundMenu(mb)
  menu = TkMenu.new(mb)
  %w(text image none top bottom left right center).each{|str|
    menu.add(:radiobutton, :label=>Tk.tk_call('string', 'totitle', str),
             :variable=>$V.ref(:COMPOUND), :value=>str,
             :command=>proc{ changeToolbars() })
  }
  menu
end

makeToolbars()

## CONTROLS
control = Tk::Tile::Frame.new($BASE)

#
# Overall theme control:
#
makeThemeControl(control).grid(:sticky=>:news, :padx=>6, :ipadx=>6)
control.grid_rowconfigure(99, :weight=>1)

def changeToolbars
  foreachWidget($TOOLBARS,
                proc{|w|
                  begin
                    w.compound($V[:COMPOUND])
                  rescue
                  end
                })
end

def scrolledWidget(parent, klass, themed, *args)
  if themed
    f = Tk::Tile::Frame.new(parent)
    t = klass.new(f, *args)
    vs = Tk::Tile::Scrollbar.new(f)
    hs = Tk::Tile::Scrollbar.new(f)
  else
    f = TkFrame.new(parent)
    t = klass.new(f, *args)
    vs = TkScrollbar.new(f)
    hs = TkScrollbar.new(f)
  end
  t.yscrollbar(vs)
  t.xscrollbar(hs)

  TkGrid.configure(t, vs, :sticky=>:news)
  TkGrid.configure(hs, 'x', :sticky=>:news)
  TkGrid.rowconfigure(f, 0, :weight=>1)
  TkGrid.columnconfigure(f, 0, :weight=>1)

  [f, t]
end

#
# Notebook demonstration:
#
def makeNotebook
  nb = Tk::Tile::Notebook.new($BASE, :padding=>6)
  nb.enable_traversal
  client = Tk::Tile::Frame.new(nb)
  nb.add(client, :text=>'Demo', :underline=>0)
  nb.select(client)

  scales = Tk::Tile::Frame.new(nb)
  nb.add(scales, :text=>'Scales')
  combo = Tk::Tile::Frame.new(nb)
  nb.add(combo, :text=>'Combobox', :underline=>7)
  tree = Tk::Tile::Frame.new(nb)
  nb.add(tree, :text=>'Tree')
  others = Tk::Tile::Frame.new(nb)
  nb.add(others, :text=>'Others', :underline=>4)

  [nb, client, scales, combo, tree, others]
end

nb, client, scales, combo, tree, others = makeNotebook()

#
# Side-by side check, radio, and menu button comparison:
#
def fillMenu(menu)
  %w(above below left right flush).each{|dir|
    menu.add(:command, :label=>Tk.tk_call('string', 'totitle', dir),
             :command=>proc{ menu.winfo_parent.direction(dir) })
  }
  menu.add(:cascade, :label=>'Submenu', :menu=>(submenu = TkMenu.new(menu)))
  submenu.add(:command, :label=>'Subcommand 1')
  submenu.add(:command, :label=>'Subcommand 2')
  submenu.add(:command, :label=>'Subcommand 3')

  menu.add(:separator)
  menu.add(:command, :label=>'Quit', :command=>proc{Tk.root.destroy})
end

l = Tk::Tile::Labelframe.new(client, :text=>'Themed', :padding=>6)
r = TkLabelframe.new(client, :text=>'Standard', :padx=>6, :pady=>6)

## Styled frame
cb = Tk::Tile::Checkbutton.new(l, :text=>'Checkbutton',
                               :variable=>$V.ref(:SELECTED), :underline=>2)
rb1 = Tk::Tile::Radiobutton.new(l, :text=>'One', :variable=>$V.ref(:CHOICE),
                                :value=>1, :underline=>0)
rb2 = Tk::Tile::Radiobutton.new(l, :text=>'Two', :variable=>$V.ref(:CHOICE),
                                :value=>2)
rb3 = Tk::Tile::Radiobutton.new(l, :text=>'Three',
                                :variable=>$V.ref(:CHOICE),
                                :value=>3, :underline=>0)
btn = Tk::Tile::Button.new(l, :text=>'Button', :underline=>0)

mb = Tk::Tile::Menubutton.new(l, :text=>'Menubutton', :underline=>2)
m = TkMenu.new(mb)
mb.menu(m)
fillMenu(m)

$entryText = TkVariable.new('Entry widget')
e = Tk::Tile::Entry.new(l, :textvariable=>$entryText)
e.selection_range(6, :end)

ltext_f, ltext = scrolledWidget(l, TkText, true,
                                :width=>12, :height=>5, :wrap=>:none)
# NOTE TO MAINTAINERS:
# The checkbuttons are -sticky ew / -expand x  on purpose:
# it demonstrates one of the differences between TCheckbuttons
# and standard checkbuttons.
#
Tk.grid(cb, :sticky=>:ew)
Tk.grid(rb1, :sticky=>:ew)
Tk.grid(rb2, :sticky=>:ew)
Tk.grid(rb3, :sticky=>:ew)
Tk.grid(btn, :sticky=>:ew, :padx=>2, :pady=>2)
Tk.grid(mb, :sticky=>:ew, :padx=>2, :pady=>2)
Tk.grid(e, :sticky=>:ew, :padx=>2, :pady=>2)
Tk.grid(ltext_f, :sticky=>:news)

TkGrid.columnconfigure(l, 0, :weight=>1)
TkGrid.rowconfigure(l, 7, :weight=>1) # text widget (grid is a PITA)

## Orig frame
cb = TkCheckbutton.new(r, :text=>'Checkbutton', :variable=>$V.ref(:SELECTED))
rb1 = TkRadiobutton.new(r, :text=>'One',
                        :variable=>$V.ref(:CHOICE), :value=>1)
rb2 = TkRadiobutton.new(r, :text=>'Two', :variable=>$V.ref(:CHOICE),
                        :value=>2, :underline=>1)
rb3 = TkRadiobutton.new(r, :text=>'Three',
                        :variable=>$V.ref(:CHOICE), :value=>3)
btn = TkButton.new(r, :text=>'Button')

mb = TkMenubutton.new(r, :text=>'Menubutton', :underline=>3, :takefocus=>true)
m = TkMenu.new(mb)
mb.menu(m)
$V[:rmbIndicatoron] = mb.indicatoron
m.add(:checkbutton, :label=>'Indicator?', #'
      :variable=>$V.ref(:rmbIndicatoron),
      :command=>proc{mb.indicatoron($V[:rmbIndicatoron])})
m.add(:separator)
fillMenu(m)

e = TkEntry.new(r, :textvariable=>$entryText)

rtext_f, rtext = scrolledWidget(r, TkText, false,
                                :width=>12, :height=>5, :wrap=>:none)

Tk.grid(cb, :sticky=>:ew)
Tk.grid(rb1, :sticky=>:ew)
Tk.grid(rb2, :sticky=>:ew)
Tk.grid(rb3, :sticky=>:ew)
Tk.grid(btn, :sticky=>:ew, :padx=>2, :pady=>2)
Tk.grid(mb, :sticky=>:ew, :padx=>2, :pady=>2)
Tk.grid(e, :sticky=>:ew, :padx=>2, :pady=>2)
Tk.grid(rtext_f, :sticky=>:news)

TkGrid.columnconfigure(l, 0, :weight=>1)
TkGrid.rowconfigure(l, 7, :weight=>1) # text widget (grid is a PITA)

Tk.grid(l, r, :sticky=>:news, :padx=>6, :pady=>6)
TkGrid.rowconfigure(client, 0, :weight=>1)
TkGrid.columnconfigure(client, [0, 1], :weight=>1)

#
# Add some text to the text boxes:
#
msgs = [
"The cat crept into the crypt, crapped and crept out again",
"Peter Piper picked a peck of pickled peppers",
"How much wood would a woodchuck chuck if a woodchuck could chuck wood",
"He thrusts his fists against the posts and still insists he sees the ghosts",
"Who put the bomb in the bom-b-bom-b-bom,",
"Is this your sister's sixth zither, sir?",
"Who put the ram in the ramalamadingdong?",
"I am not the pheasant plucker, I'm the pheasant plucker's mate."
]

nmsgs = msgs.size
(0...50).each{|n|
  msg = msgs[n % nmsgs]
  ltext.insert(:end, "#{n}: #{msg}\n")
  rtext.insert(:end, "#{n}: #{msg}\n")
}
#
# Scales and sliders pane:
#
l = Tk::Tile::Labelframe.new(scales, :text=>'Themed', :padding=>6)
r = TkLabelframe.new(scales, :text=>'Standard', :padx=>6, :pady=>6)

if version?('0.6')

  # thremed frame
  scale = Tk::Tile::Scale.new(l, :orient=>:horizontal, :from=>0, :to=>100,
          :variable=>$V.ref(:SCALE))
  vscale = Tk::Tile::Scale.new(l, :orient=>:vertical, :from=>0, :to=>100,
          :variable=>$V.ref(:VSCALE))
  progress = Tk::Tile::Progressbar.new(l, :orient=>:horizontal, :maximum=>100)
  vprogress = Tk::Tile::Progressbar.new(l, :orient=>:vertical, :maximum=>100)

  if true
    def progress.inverted(w, value)
      if w.mode == 'indeterminate'
        w.value(value)
      else
        w.value(w.maximum - value)
      end
    end
    scale.command {|value| progress.value(value)}
    vscale.command {|value| progress.inverted(vprogress, value) }
  else
    # This would also work, but the Tk scale widgets
    # in the right hand pane cause some interference when
    # in autoincrement/indeterminate mode.
    #
    progress.variable $V.ref(:SCALE)
    vprogress.variable $V.ref(:VSCALE)
  end

  scale.set(50)
  vscale.set(50)

  lmode = Tk::Tile::Label.new(l, :text=>'Progress bar mode')
  pbmode0 = Tk::Tile::Radiobutton.new(l, :variable=>$V.ref(:PBMODE),
          :text=>'determinate', :value=>'determinate',
          :command=>proc{pbMode(progress, vprogress)})
  pbmode1 = Tk::Tile::Radiobutton.new(l, :variable=>$V.ref(:PBMODE),
          :text=>'indeterminate', :value=>'indeterminate',
          :command=>proc{pbMode(progress, vprogress)})
  def pbMode(progress, vprogress)
    if vprogress.mode != $V[:PBMODE]
      vprogress.value(vprogress.maximum - vprogress.value)
    end

    progress.mode $V[:PBMODE]
    vprogress.mode $V[:PBMODE]
  end

  start = Tk::Tile::Button.new(l, :text=>"Start",
          :command=>proc{pbStart(progress, vprogress)})
  def pbStart(progress, vprogress)
    # $V[:PBMODE] = 'indeterminate'
    pbMode(progress, vprogress)
    progress.start 10
    vprogress.start
  end

  stop = Tk::Tile::Button.new(l, :text=>'Stop',
          :command=>proc{pbStop(progress, vprogress)})
  def pbStop(progress, vprogress)
    progress.stop
    vprogress.stop
  end

  Tk.grid(scale, :columnspan=>2, :sticky=>'ew')
  Tk.grid(progress, :columnspan=>2, :sticky=>'ew')
  Tk.grid(vscale, vprogress, :sticky=>'nws')

  Tk.grid(lmode, :sticky=>'we', :columnspan=>2)
  Tk.grid(pbmode0, :sticky=>'we', :columnspan=>2)
  Tk.grid(pbmode1, :sticky=>'we', :columnspan=>2)
  Tk.grid(start, :sticky=>'we', :columnspan=>2)
  Tk.grid(stop, :sticky=>'we', :columnspan=>2)

  l.grid_columnconfigure(0, :weight=>1)
  l.grid_columnconfigure(1, :weight=>1)
  l.grid_rowconfigure(99, :weight=>1)

  # standard frame
  TkScale.new(r, :orient=>:horizontal, :from=>0, :to=>100,
          :variable=>$V.ref(:SCALE)).grid(:sticky=>'news')
  TkScale.new(r, :orient=>:vertical, :from=>0, :to=>100,
          :variable=>$V.ref(:VSCALE)).grid(:sticky=>'nws')

  r.grid_columnconfigure(0, :weight=>1)
  r.grid_columnconfigure(1, :weight=>1)
  r.grid_rowconfigure(99, :weight=>1)

else # tile 0.5 or earlier

  # themed frame
  scale = Tk::Tile::Scale.new(l, :variable=>$V.ref(:SCALE),
          :orient=>:horizontal, :from=>0, :to=>100)
  vscale = Tk::Tile::Scale.new(l, :variable=>$V.ref(:VSCALE),
          :orient=>:vertical, :from=>-25, :to=>25)

  progress = Tk::Tile::Progress.new(l,
          :orient=>:horizontal, :from=>0, :to=>100)
  vprogress = Tk::Tile::Progress.new(l,
          :orient=>:vertical, :from=>-25, :to=>25)

  if true
    scale.command{|value| progress.set(value)}
    vscale.command{|value| vprogress.set(value)}
  else # this would also work. (via TkVariable#trace)
    v1 = scale.variable
    v2 = vscale.variable
    v1.trace('w', proc{ progress.set(v1.value) })
    v2.trace('w', proc{ vprogress.set(v2.value) })
  end

  Tk.grid(scale, :columnspan=>2, :sticky=>:ew)
  Tk.grid(progress, :columnspan=>2, :sticky=>:ew)
  Tk.grid(vscale, vprogress, :sticky=>:nws)
  TkGrid.columnconfigure(l, 0, :weight=>1)
  TkGrid.columnconfigure(l, 1, :weight=>1)

  # standard frame
  TkScale.new(r, :variable=>$V.ref(:SCALE),
          :orient=>:horizontal, :from=>0, :to=>100).grid(:sticky=>'news')
  TkScale.new(r, :variable=>$V.ref(:VSCALE),
          :orient=>:vertical, :from=>-25, :to=>25).grid(:sticky=>'nws')

  TkGrid.columnconfigure(r, 0, :weight=>1)
  TkGrid.columnconfigure(r, 1, :weight=>1)
end

# layout frames
Tk.grid(l, r, :sticky=>'nwes', :padx=>6, :pady=>6)
scales.grid_columnconfigure(0, :weight=>1)
scales.grid_columnconfigure(1, :weight=>1)
scales.grid_rowconfigure(0, :weight=>1)

#
# Command box:
#
cmd = Tk::Tile::Frame.new($BASE)
b_close = Tk::Tile::Button.new(cmd, :text=>'Close',
                               :underline=>0, :default=>:normal,
                               :command=>proc{Tk.root.destroy})
b_help = Tk::Tile::Button.new(cmd, :text=>'Help', :underline=>0,
                              :default=>:normal, :command=>proc{showHelp()})
Tk.grid('x', b_close, b_help, :pady=>[6, 4], :padx=>4)
TkGrid.columnconfigure(cmd, 0, :weight=>1)

#
# Set up accelerators:
#
$ROOT.bind('KeyPress-Escape', proc{Tk.event_generate(b_close, '<Invoke>')})
$ROOT.bind('<Help>', proc{Tk.event_generate(b_help, '<Invoke>')})
Tk::Tile::KeyNav.enableMnemonics($ROOT)
Tk::Tile::KeyNav.defaultButton(b_help)

Tk.grid($TOOLBARS[0], '-', :sticky=>:ew)
Tk.grid($TOOLBARS[1], '-', :sticky=>:ew)
Tk.grid(control,      nb,  :sticky=>:news)
Tk.grid(cmd,          '-', :sticky=>:ew)
TkGrid.columnconfigure($ROOT, 1, :weight=>1)
TkGrid.rowconfigure($ROOT, 2, :weight=>1)

#
# Add a menu
#
menu = TkMenu.new($BASE)
$ROOT.menu(menu)
m_file = TkMenu.new(menu, :tearoff=>0)
menu.add(:cascade, :label=>'File', :underline=>0, :menu=>m_file)
m_file.add(:command, :label=>'Open', :underline=>0,
           :compound=>:left, :image=>$ICON['open'])
m_file.add(:command, :label=>'Save', :underline=>0,
           :compound=>:left, :image=>$ICON['save'])
m_file.add(:separator)
m_f_test = TkMenu.new(menu, :tearoff=>0)
m_file.add(:cascade, :label=>'Test submenu', :underline=>0, :menu=>m_f_test)
m_file.add(:checkbutton, :label=>'Text check', :underline=>5,
           :variable=>$V.ref(:MENUCHECK1))
m_file.insert(:end, :separator)

if Tk.windowingsystem != 'x11'
  TkConsole.create
  m_file.insert(:end, :checkbutton, :label=>'Console', :underline=>5,
                :variable=>$V.ref(:CONSOLE), :command=>proc{toggle_console()})
  def toggle_console
    if TkComm.bool($V[:CONSOLE])
      TkConsole.show
    else
      TkConsole.hide
    end
  end
end

m_file.add(:command, :label=>'Exit', :underline=>1,
           :command=>proc{Tk.event_generate(b_close, '<Invoke>')})

%w(One Two Three Four).each{|lbl|
  m_f_test.add(:radiobutton, :label=>lbl, :variable=>$V.ref(:MENURADIO1))
}

# Add Theme menu.
#
menu.add(:cascade, :label=>'Theme', :underline=>3,
         :menu=>makeThemeMenu(menu))

setTheme($V[:THEME])

#
# Combobox demo pane:
#
values = %w(list abc def ghi jkl mno pqr stu vwx yz)
2.times {|i|
  cb = Tk::Tile::Combobox.new(
    combo, :values=>values, :textvariable=>$V.ref(:COMBO))
  cb.pack(:side=>:top, :padx=>2, :pady=>2, :expand=>false, :fill=>:x)
  if i == 1
    cb.ttk_state :readonly
    begin
      cb.current = 3 # ignore if unsupported (tile0.4)
    rescue
    end
  end
}

#
# Treeview widget demo pane:
#
if version?('0.5')

  treeview = nil # avoid 'undefined' error
  scrollbar = Tk::Tile::Scrollbar.new(tree,
      :command=>proc{|*args| treeview.yview(*args)})
  treeview = Tk::Tile::Treeview.new(tree, :columns=>%w(Class), :padding=>4,
      :yscrollcommand=>proc{|*args| scrollbar.set(*args)})

  Tk.grid(treeview, scrollbar, :sticky=>'news')
  tree.grid_columnconfigure(0, :weight=>1)
  tree.grid_rowconfigure(0, :weight=>1)
  tree.grid_propagate(0)

  # Add initial tree node:
  # Later nodes will be added in <<TreeviewOpen>> binding.
  treeview.insert('', 0, :id=>'.', :text=>'Main Window', :open=>false,
      :values=>[TkWinfo.classname('.')])
  treeview.headingconfigure('#0', :text=>'Widget')
  treeview.headingconfigure('Class', :text=>'Class')
  treeview.bind('<TreeviewOpen>', proc{fillTree(treeview)})

  def fillTree(treeview)
    id = treeview.focus_item
    unless TkWinfo.exist?(id)
      treeview.delete(id)
    end
    # Replace tree item children with current list of child windows.
    treeview.delete(treeview.children(id))
    for child in TkWinfo.children(id)
      treeview.insert(id, :end, :id=>child, :text=>TkWinfo.appname(child),
          :open=>false, :values=>[TkWinfo.classname(child)])
      unless TkWinfo.children(child).empty?
        # insert dummy child to show [+] indicator
        treeview.insert(child, :end)
      end
    end
  end

else
  Tk::Tile::Label.new(tree,
      :text=>'Treeview is supported on tile 0.5 or later...').pack
end

#
# Other demos:
#
$Timers = {:StateMonitor=>nil, :FocusMonitor=>nil}

begin
  msg = Tk::Tile::Label.new(others, :justify=>:left, :wraplength=>300)
rescue
  msg = TkMessage.new(others, :aspect=>200)
end

$Desc = {}

showDescription = TkBindTag.new
showDescription.bind('Enter', proc{|w| msg.text($Desc[w.path])}, '%W')
showDescription.bind('Leave', proc{|w| msg.text('')}, '%W')

[
  [ :trackStates, "Widget states...",
    "Display/modify widget state bits" ],

  [ :scrollbarResizeDemo,  "Scrollbar resize behavior...",
    "Shows how Tile and standard scrollbars differ when they're sized too large" ],

  [ :trackFocus, "Track keyboard focus..." ,
    "Display the name of the widget that currently has focus" ],

  [ :repeatDemo, "Repeating buttons...",
    "Demonstrates custom classes (see demos/repeater.tcl)" ]

].each{|demo_cmd, label, description|
  b = Tk::Tile::Button.new(others, :text=>label,
                           :command=>proc{ self.__send__(demo_cmd) })
  $Desc[b.path] = description
  b.bindtags <<= showDescription

  b.pack(:side=>:top, :expand=>false, :fill=>:x, :padx=>6, :pady=>6)
}

msg.pack(:side=>:bottom, :expand=>true, :fill=>:both)


#
# Scrollbar resize demo:
#
$scrollbars = nil

def scrollbarResizeDemo
  if $scrollbars
    begin
      $scrollbars.destroy
    rescue
    end
  end
  $scrollbars = TkToplevel.new(:title=>'Scrollbars', :geometry=>'200x200')
  f = TkFrame.new($scrollbars, :height=>200)
  tsb = Tk::Tile::Scrollbar.new(f, :command=>proc{|*args| sbstub(tsb, *args)})
  sb = TkScrollbar.new(f, :command=>proc{|*args| sbstub(sb, *args)})
  Tk.grid(tsb, sb, :sticky=>:news)

  sb.set(0, 0.5)  # prevent backwards-compatibility mode for old SB

  f.grid_columnconfigure(0, :weight=>1)
  f.grid_columnconfigure(1, :weight=>1)
  f.grid_rowconfigure(0, :weight=>1)

  f.pack(:expand=>true, :fill=>:both)
end

#
# Track focus demo:
#
$FocusInf = TkVariable.new_hash
$focus = nil

def trackFocus
  if $focus
    begin
      $focus.destroy
    rescue
    end
  end
  $focus = TkToplevel.new(:title=>'Keyboard focus')
  i = 0
  [
    ["Focus widget:", :Widget],
    ["Class:", :WidgetClass],
    ["Next:", :WidgetNext],
    ["Grab:", :Grab],
    ["Status:", :GrabStatus]
  ].each{|label, var_index|
    Tk.grid(Tk::Tile::Label.new($focus, :text=>label, :anchor=>:e),
            Tk::Tile::Label.new($focus,
                                :textvariable=>$FocusInf.ref(var_index),
                                :width=>40, :anchor=>:w, :relief=>:groove),
            :sticky=>:ew)
    i += 1
  }
  $focus.grid_columnconfigure(1, :weight=>1)
  $focus.grid_rowconfigure(i, :weight=>1)

  $focus.bind('Destroy', proc{Tk.after_cancel($Timers[:FocusMonitor])})
  focusMonitor
end

def focusMonitor
  $FocusInf[:Widget] = focus_win = Tk.focus
  if focus_win
    $FocusInf[:WidgetClass] = focus_win.winfo_classname
    $FocusInf[:WidgetNext] = Tk.focus_next(focus_win)
  else
    $FocusInf[:WidgetClass] = $FocusInf[:WidgetNext] = ''
  end

  $FocusInf[:Grab] = grab_wins = Tk.current_grabs
  unless grab_wins.empty?
    $FocusInf[:GrabStatus] = grab_wins[0].grab_status
  else
    $FocusInf[:GrabStatus] = ''
  end

  $Timers[:FocusMonitor] = Tk.after(200, proc{ focusMonitor() })
end

#
# Widget state demo:
#
$Widget = TkVariable.new

TkBindTag::ALL.bind('Control-Shift-ButtonPress-1',
                    proc{|w|
                      $Widget.value = w
                      updateStates()
                      Tk.callback_break
                    }, '%W')
$states_list = %w(active disabled focus pressed selected
                  background indeterminate invalid default)
$states_btns = {}
$states = nil

$State = TkVariable.new_hash

def trackStates
  if $states
    begin
      $state.destroy
    rescue
    end
  end
  $states = TkToplevel.new(:title=>'Widget states')

  l_inf = Tk::Tile::Label.new($states, :text=>"Press Control-Shift-Button-1 on any widget")

  l_lw = Tk::Tile::Label.new($states, :text=>'Widget:',
                             :anchor=>:e, :relief=>:groove)
  l_w = Tk::Tile::Label.new($states, :textvariable=>$Widget,
                             :anchor=>:w, :relief=>:groove)

  Tk.grid(l_inf, '-', :sticky=>:ew, :padx=>6, :pady=>6)
  Tk.grid(l_lw, l_w, :sticky=>:ew)

  $states_list.each{|st|
    cb = Tk::Tile::Checkbutton.new($states, :text=>st,
                                   :variable=>$State.ref(st),
                                   :command=>proc{ changeState(st) })
    $states_btns[st] = cb
    Tk.grid('x', cb, :sticky=>:nsew)
  }

  $states.grid_columnconfigure(1, :weight=>1)

  f_cmd = Tk::Tile::Frame.new($states)
  Tk.grid('x', f_cmd, :sticky=>:nse)

  b_close = Tk::Tile::Button.new(f_cmd, :text=>'Close',
                                 :command=>proc{ $states.destroy })
  Tk.grid('x', b_close, :padx=>4, :pady=>[6,4])
  f_cmd.grid_columnconfigure(0, :weight=>1)

  $states.bind('KeyPress-Escape', proc{Tk.event_generate(b_close, '<Invoke>')})

  $states.bind('Destroy', proc{Tk.after_cancel($Timers[:StateMonitor])})
  stateMonitor()
end

def stateMonitor
  updateStates() if $Widget.value != ''
  $Timers[:StateMonitor] = Tk.after(200, proc{ stateMonitor() })
end

def updateStates
  $states_list.each{|st|
    begin
      $State[st] = $Widget.window.ttk_instate(st)
    rescue
      $states_btns[st].ttk_state('disabled')
    else
      $states_btns[st].ttk_state('!disabled')
    end
  }
end

def changeState(st)
  if $Widget.value != ''
    if $State.bool_element(st)
      $Widget.window.ttk_state(st)
    else
      $Widget.window.ttk_state("!#{st}")
    end
  end
end

#
# Repeating buttons demo:
#
def repeatDemo
  if defined?($repeatDemo) && $repeatDemo.exist?
    $repeatDemo.deiconify; return
  end
  $repeatDemo = TkToplevel.new(:title=>'Repeating button')

  f = Tk::Tile::Frame.new($repeatDemo)
  b = Tk::Tile::Button.new(f, :class=>'Repeater', :text=>'Press and hold')
  if version?('0.6')
    p = Tk::Tile::Progressbar.new(f, :orient=>:horizontal, :maximum=>10)
  else # progressbar is not supported
    p = Tk::Tile::Progress.new(f, :orient=>:horizontal, :from=>0, :to=>10)
    def p.step
      i = self.get + 1
      i = self.from if i > self.to
      self.set(i)
    end
  end
  b.command {p.step}

  b.pack(:side=>:left, :expand=>false, :fill=>:none, :padx=>6, :pady=>6)
  p.pack(:side=>:right, :expand=>true, :fill=>:x, :padx=>6, :pady=>6)
  f.pack(:expand=>true, :fill=>:both)
end

Tk.mainloop
