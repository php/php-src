# -*- coding: utf-8 -*-
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
    text("このウィンドウにはカスケードメニューを持つメニューバーが付けられています。Command+x ('x'はコマンドキーシンボルに続けて表示されている文字です) とタイプすることによっても項目の機能を呼び出すことができます。最後のメニューは、マウスでウィンドウの外にドラッグすることによって、独立したパレットとなるように切り放すことが可能です。")
  else
    text("このウィンドウにはカスケードメニューを持つメニューバーが付けられています。Alt+x ('x'はメニュー上で下線が引かれた文字です) とタイプすることによってもメニューを呼び出すことができます。矢印キーを使って、メニュー間を移動することも可能です。メニューが表示されている時には、現在位置の項目をスペースキーで選択したり、下線が引かれた文字を入力することでその項目を選択したりすることができます。もし項目にアクセラレータの指定がなされていたならば、その指定されたキー入力を行うことで、メニューを表示させることなく直接その項目の機能を呼び出せます。最後のメニューは、メニューの最初の項目を選択することによって、独立したパレットとなるように切り放すことが可能です。")
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
    #text '了解'
    text '閉じる'
    command proc{
      tmppath = $menu84_demo
      $menu84_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'コード参照'
    command proc{showCode 'menu84'}
  }.pack('side'=>'left', 'expand'=>'yes')
}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')


# create menu frame
$menu84_frame = TkMenu.new($menu84_demo, 'tearoff'=>false)

# menu
TkMenu.new($menu84_frame, 'tearoff'=>false) {|m|
  $menu84_frame.add('cascade', 'label'=>'File', 'menu'=>m, 'underline'=>0)
  add('command', 'label'=>'Open...', 'command'=>proc{fail 'これは単なるデモですから、"Open..." 項目の機能は特に定義されてはいません。'})
  add('command', 'label'=>'New', 'command'=>proc{fail 'これは単なるデモですから、"New" 項目の機能は特に定義されてはいません。'})
  add('command', 'label'=>'Save', 'command'=>proc{fail 'これは単なるデモですから、"Save" 項目の機能は特に定義されてはいません。'})
  add('command', 'label'=>'Save As...', 'command'=>proc{fail 'これは単なるデモですから、"Save As..." 項目の機能は特に定義されてはいません。'})
  add('separator')
  add('command', 'label'=>'Print Setup...', 'command'=>proc{fail 'これは単なるデモですから、"Print Setup..." 項目の機能は特に定義されてはいません。'})
  add('command', 'label'=>'Print...', 'command'=>proc{fail 'これは単なるデモですから、"Print..." 項目の機能は特に定義されてはいません。'})
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
    m.add('cascade', 'label'=>'Check button',
          'menu'=>cascade_check, 'underline'=>0)
    oil = TkVariable.new(0)
    add('check', 'label'=>'オイル検査', 'variable'=>oil)
    trans = TkVariable.new(0)
    add('check', 'label'=>'トランスミッション検査', 'variable'=>trans)
    brakes = TkVariable.new(0)
    add('check', 'label'=>'ブレーキ検査', 'variable'=>brakes)
    lights = TkVariable.new(0)
    add('check', 'label'=>'ライト検査', 'variable'=>lights)
    add('separator')
    add('command', 'label'=>'Show current values',
        'command'=>proc{showVars($menu84_demo,
                                 ['オイル', oil],
                                 ['トランスミッション', trans],
                                 ['ブレーキ', brakes],
                                 ['ライト', lights])} )
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
    add('command', 'label'=>'現在値の表示',
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
                                   'text'=>'あなたが選択したメニュー項目は、文字列の代わりにビットマップイメージで項目を表示したものです。それ以外の点では、ほかのメニュー項目との間で特に違いがあるわけではありません。',
                                   'bitmap'=>'', 'default'=>0,
                                   'buttons'=>'閉じる')} )
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
                                  'message'=>'あなたが選択したメニュー項目は、ビットマップイメージと文字列とを同時に一つの項目に表示するようにしたものです。それ以外の点では、ほかのメニュー項目との間で特に違いがあるわけではありません。',
                                  'buttons'=>['了解'], 'bitmap'=>'')
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
