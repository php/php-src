#
# menus widget demo (called by 'widget')
#

# toplevel widget
if defined?($menu84_demo) && $menu84_demo
  $menu84_demo.destroy
  $menu84_demo = nil
end

# demo toplevel widget
$menu84_demo = TkToplevel.new {|w|
  title("File Selection Dialogs")
  iconname("menu84")
  positionWindow(w)
}

base_frame = TkFrame.new($menu84_demo).pack(:fill=>:both, :expand=>true)

begin
  windowingsystem = Tk.windowingsystem()
rescue
  windowingsystem = ""
end

# label
TkLabel.new(base_frame,'font'=>$font,'wraplength'=>'4i','justify'=>'left') {
  if $tk_platform['platform'] == 'macintosh' ||
      windowingsystem == "classic" || windowingsystem == "aqua"
    text("This window contains a menubar with cascaded menus.  You can invoke entries with an accelerator by typing Command+x, where \"x\" is the character next to the command key symbol. The rightmost menu can be torn off into a palette by dragging outside of its bounds and releasing the mouse.")
  else
    text("This window contains a menubar with cascaded menus.  You can post a menu from the keyboard by typing Alt+x, where \"x\" is the character underlined on the menu.  You can then traverse among the menus using the arrow keys.  When a menu is posted, you can invoke the current entry by typing space, or you can invoke any entry by typing its underlined character.  If a menu entry has an accelerator, you can invoke the entry without posting the menu just by typing the accelerator. The rightmost menu can be torn off into a palette by selecting the first item in the menu.")
  end
}.pack('side'=>'top')


menustatus = TkVariable.new("    ")
TkFrame.new(base_frame) {|frame|
  TkLabel.new(frame, 'textvariable'=>menustatus, 'relief'=>'sunken',
              'bd'=>1, 'font'=>['Helvetica', '10'],
              'anchor'=>'w').pack('side'=>'left', 'padx'=>2,
                                  'expand'=>true, 'fill'=>'both')
  pack('side'=>'bottom', 'fill'=>'x', 'pady'=>2)
}


# frame
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    text 'Dismiss'
    command proc{
      tmppath = $menu84_demo
      $menu84_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'Show Code'
    command proc{showCode 'menu84'}
  }.pack('side'=>'left', 'expand'=>'yes')
}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')


# create menu frame
$menu84_frame = TkMenu.new($menu84_demo, 'tearoff'=>false)

# menu
TkMenu.new($menu84_frame, 'tearoff'=>false) {|m|
  $menu84_frame.add('cascade', 'label'=>'File', 'menu'=>m, 'underline'=>0)
  add('command', 'label'=>'Open...', 'command'=>proc{fail 'this is just a demo: no action has been defined for the "Open..." entry'})
  add('command', 'label'=>'New', 'command'=>proc{fail 'this is just a demo: no action has been defined for the "New" entry'})
  add('command', 'label'=>'Save', 'command'=>proc{fail 'this is just a demo: no action has been defined for the "Save" entry'})
  add('command', 'label'=>'Save As...', 'command'=>proc{fail 'this is just a demo: no action has been defined for the "Save As..." entry'})
  add('separator')
  add('command', 'label'=>'Print Setup...', 'command'=>proc{fail 'this is just a demo: no action has been defined for the "Print Setup..." entry'})
  add('command', 'label'=>'Print...', 'command'=>proc{fail 'this is just a demo: no action has been defined for the "Print..." entry'})
  add('separator')
  add('command', 'label'=>'Dismiss Menus Demo', 'command'=>proc{$menu84_demo.destroy})
}

if $tk_platform['platform'] == 'macintosh' ||
    windowingsystem = "classic" || windowingsystem = "aqua"
  modifier = 'Command'
elsif $tk_platform['platform'] == 'windows'
  modifier = 'Control'
else
  modifier = 'Meta'
end

TkMenu.new($menu84_frame, 'tearoff'=>false) {|m|
  $menu84_frame.add('cascade', 'label'=>'Basic', 'menu'=>m, 'underline'=>0)
  add('command', 'label'=>'Long entry that does nothing')
  ['A','B','C','D','E','F','G'].each{|c|
    add('command', 'label'=>"Print letter \"#{c}\"",
        'underline'=>14, 'accelerator'=>"Meta+#{c}",
        'command'=>proc{print c,"\n"}, 'accelerator'=>"#{modifier}+#{c}")
    $menu84_demo.bind("#{modifier}-#{c.downcase}", proc{print c,"\n"})
  }
}

TkMenu.new($menu84_frame, 'tearoff'=>false) {|m|
  $menu84_frame.add('cascade', 'label'=>'Cascades', 'menu'=>m, 'underline'=>0)
  add('command', 'label'=>'Print hello',
      'command'=>proc{print "Hello\n"},
      'accelerator'=>"#{modifier}+H", 'underline'=>6)
  $menu84_demo.bind("#{modifier}-h", proc{print "Hello\n"})
  add('command', 'label'=>'Print goodbye',
      'command'=>proc{print "Goodbye\n"},
      'accelerator'=>"#{modifier}+G", 'underline'=>6)
  $menu84_demo.bind("#{modifier}-g", proc{print "Goodbye\n"})

  TkMenu.new(m, 'tearoff'=>false) {|cascade_check|
    m.add('cascade', 'label'=>'Check buttons',
          'menu'=>cascade_check, 'underline'=>0)
    oil = TkVariable.new(0)
    add('check', 'label'=>'Oil checked', 'variable'=>oil)
    trans = TkVariable.new(0)
    add('check', 'label'=>'Transmission checked', 'variable'=>trans)
    brakes = TkVariable.new(0)
    add('check', 'label'=>'Brakes checked', 'variable'=>brakes)
    lights = TkVariable.new(0)
    add('check', 'label'=>'Lights checked', 'variable'=>lights)
    add('separator')
    add('command', 'label'=>'Show current values',
        'command'=>proc{showVars($menu84_demo,
                                 ['oil', oil],
                                 ['trans', trans],
                                 ['brakes', brakes],
                                 ['lights', lights])} )
    invoke 1
    invoke 3
  }

  TkMenu.new(m, 'tearoff'=>false) {|cascade_radio|
    m.add('cascade', 'label'=>'Radio buttons',
          'menu'=>cascade_radio, 'underline'=>0)
    pointSize = TkVariable.new
    add('radio', 'label'=>'10 point', 'variable'=>pointSize, 'value'=>10)
    add('radio', 'label'=>'14 point', 'variable'=>pointSize, 'value'=>14)
    add('radio', 'label'=>'18 point', 'variable'=>pointSize, 'value'=>18)
    add('radio', 'label'=>'24 point', 'variable'=>pointSize, 'value'=>24)
    add('radio', 'label'=>'32 point', 'variable'=>pointSize, 'value'=>32)
    add('separator')
    style = TkVariable.new
    add('radio', 'label'=>'Roman', 'variable'=>style, 'value'=>'roman')
    add('radio', 'label'=>'Bold', 'variable'=>style, 'value'=>'bold')
    add('radio', 'label'=>'Italic', 'variable'=>style, 'value'=>'italic')
    add('separator')
    add('command', 'label'=>'Show current values',
        'command'=>proc{showVars($menu84_demo,
                                 ['pointSize', pointSize],
                                 ['style', style])} )
    invoke 1
    invoke 7
  }
}

TkMenu.new($menu84_frame, 'tearoff'=>false) {|m|
  $menu84_frame.add('cascade', 'label'=>'Icons', 'menu'=>m, 'underline'=>0)
  add('command', 'hidemargin'=>1,
      'bitmap'=>'@'+[$demo_dir,'..',
                      'images','pattern.xbm'].join(File::Separator),
      'command'=>proc{TkDialog.new('title'=>'Bitmap Menu Entry',
                                   'text'=>'The menu entry you invoked displays a bitmap rather than a text string.  Other than this, it is just like any other menu entry.',
                                   'bitmap'=>'', 'default'=>0,
                                   'buttons'=>'Dismiss')} )
  ['info', 'questhead', 'error'].each{|icon|
    add('command', 'bitmap'=>icon, 'hidemargin'=>1,
        'command'=>proc{print "You invoked the #{icon} bitmap\n"})
  }

  entryconfigure(2, :columnbreak=>true)
}

TkMenu.new($menu84_frame, 'tearoff'=>false) {|m|
  $menu84_frame.add('cascade', 'label'=>'More', 'menu'=>m, 'underline'=>0)
  [ 'An entry','Another entry','Does nothing','Does almost nothing',
    'Make life meaningful' ].each{|i|
    add('command', 'label'=>i,
        'command'=>proc{print "You invoked \"#{i}\"\n"})
  }

  m.entryconfigure('Does almost nothing',
                   'bitmap'=>'questhead', 'compound'=>'left',
                   'command'=>proc{
                     TkDialog.new('title'=>'Compound Menu Entry',
                                  'message'=>'The menu entry you invoked'+
                                             'displays both a bitmap and '+
                                             'a text string.  Other than '+
                                             'this, it isjust like any '+
                                             'other menu entry.',
                                  'buttons'=>['OK'], 'bitmap'=>'')
                   })
}

TkMenu.new($menu84_frame) {|m|
  $menu84_frame.add('cascade', 'label'=>'Colors', 'menu'=>m, 'underline'=>0)
  ['red', 'orange', 'yellow', 'green', 'blue'].each{|c|
    add('command', 'label'=>c, 'background'=>c,
        'command'=>proc{print "You invoked \"#{c}\"\n"})
  }
}

$menu84_demo.menu($menu84_frame)

TkMenu.bind('<MenuSelect>', proc{|w|
              begin
                label = w.entrycget('active', 'label')
              rescue
                label = "    "
              end
              menustatus.value = label
              Tk.update(true)
            }, '%W')
