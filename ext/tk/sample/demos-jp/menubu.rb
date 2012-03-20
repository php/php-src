# -*- coding: utf-8 -*-
require "tkcanvas"

def optionMenu(menubutton, varName, firstValue, *rest)
  varName.value = firstValue
  configoptions = {'textvariable'=>varName,'indicatoron'=>'on',
    'relief'=>'raised','borderwidth'=>2,'highlightthickness'=>2,
    'anchor'=>'c','direction'=>'flush'}
  configoptions.each {|key, value|
    menubutton.configure(key, value)
  }
  menu = TkMenu.new(menubutton) {
    tearoff 'off'
    add 'radio', 'label'=>firstValue, 'variable'=>varName
  }
  menubutton.menu(menu)
  for i in rest
    menu.add 'radio', 'label'=>i, 'variable'=>varName
  end

  return menu
end

if defined?($menubu_demo) && $menubu_demo
  $menubu_demo.destroy
  $menubu_demo = nil
end

$menubu_demo = TkToplevel.new {|w|
  title("Menu Button Demonstration")
  iconname("menubutton")
}

positionWindow($menubu_demo)

base_frame = TkFrame.new($menubu_demo).pack(:fill=>:both, :expand=>true)

# version check
if $tk_version.to_f < 8.0

# label 生成
TkLabel.new(base_frame,'font'=>$font,'wraplength'=>'4i','justify'=>'left') {
    text("実行しようとしたスクリプトは Tk8.0 以上で利用できる機能を利用しているため、あなたの Ruby#{VERSION}/Tk#{$tk_version}#{(Tk::JAPANIZED_TK)? 'jp': ''} では正常に実行できません。よってデモの実行を中止しました。ただし、下のコード参照ボタンを押すことで、実行が中止されたスクリプトのソースを参照することは可能です。")
}.pack('side'=>'top')

# frame 生成
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    #text '了解'
    text '閉じる'
    command proc{
      tmppath = $menubu_demo
      $menubu_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'コード参照'
    command proc{showCode 'menubu'}
  }.pack('side'=>'left', 'expand'=>'yes')
}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

else ; # Tk8.x

body = TkFrame.new(base_frame)
body.pack('expand'=>'yes', 'fill'=>'both')

below = TkMenubutton.new(body) {
  text "Below"
  underline 0
  direction 'below'
  relief 'raised'
}
belowMenu = TkMenu.new(below) {
  tearoff 0
  add 'command', 'label'=>"Below menu: first item", 'command'=>proc {puts "\"You have selected the first item from the Below menu.\""}
  add 'command', 'label'=>"Below menu: second item", 'command'=>proc {puts "\"You have selected the second item from the Below menu.\""}
}
below.menu(belowMenu)
below.grid('row'=>0, 'column'=>1, 'sticky'=>'n')

below = TkMenubutton.new(body) {
  text "Below"
  underline 0
  direction 'below'
  relief 'raised'
}
belowMenu = TkMenu.new(below) {
  tearoff 0
  add 'command', 'label'=>"Below menu: first item", 'command'=>proc {puts "\"You have selected the first item from the Below menu.\""}
  add 'command', 'label'=>"Below menu: second item", 'command'=>proc {puts "\"You have selected the second item from the Below menu.\""}
}
below.menu(belowMenu)
below.grid('row'=>0, 'column'=>1, 'sticky'=>'n')

below = TkMenubutton.new(body) {
  text "Below"
  underline 0
  direction 'below'
  relief 'raised'
}
belowMenu = TkMenu.new(below) {
  tearoff 0
  add 'command', 'label'=>"Below menu: first item", 'command'=>proc {puts "\"You have selected the first item from the Below menu.\""}
  add 'command', 'label'=>"Below menu: second item", 'command'=>proc {puts "\"You have selected the second item from the Below menu.\""}
}
below.menu(belowMenu)
below.grid('row'=>0, 'column'=>1, 'sticky'=>'n')

right = TkMenubutton.new(body) {
  text "Right"
  underline 0
  direction 'right'
  relief 'raised'
}
rightMenu = TkMenu.new(right) {
  tearoff 0
  add 'command', 'label'=>"Right menu: first item", 'command'=>proc {puts "\"You have selected the first item from the Left menu.\""}
  add 'command', 'label'=>"Right menu: second item", 'command'=>proc {puts "\"You have selected the second item from the Right menu.\""}
}
right.menu(rightMenu)
right.grid('row'=>1, 'column'=>0, 'sticky'=>'w')

left = TkMenubutton.new(body) {
  text "Left"
  underline 0
  direction 'left'
  relief 'raised'
}
leftMenu = TkMenu.new(left) {
  tearoff 0
  add 'command', 'label'=>"Left menu: first item", 'command'=>proc {puts "\"You have selected the first item from the Left menu.\""}
  add 'command', 'label'=>"Left menu: second item", 'command'=>proc {puts "\"You have selected the second item from the Left menu.\""}
}
left.menu(leftMenu)
left.grid('row'=>1, 'column'=>2, 'sticky'=>'e')

center = TkFrame.new(body) {
  grid('row'=>1, 'column'=>1, 'sticky'=>'news')
}

above = TkMenubutton.new(body) {
  text "Above"
  underline 0
  direction 'above'
  relief 'raised'
}
aboveMenu = TkMenu.new(above) {
  tearoff 0
  add 'command', 'label'=>"Above menu: first item", 'command'=>proc {puts "\"You have selected the first item from the Above menu.\""}
  add 'command', 'label'=>"Above menu: second item", 'command'=>proc {puts "\"You have selected the second item from the Above menu.\""}
}
above.menu(aboveMenu)
above.grid('row'=>2, 'column'=>1, 'sticky'=>'s')

center = TkFrame.new(body) {
  grid('row'=>1, 'column'=>1, 'sticky'=>'news')
}

TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    #text '了解'
    text '閉じる'
    command proc {
      tmppath = $menubu_demo
      $menubu_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'コード参照'
    command proc { showCode 'menubu' }
  }.pack('side'=>'left', 'expand'=>'yes')
}.pack('side'=>'bottom', 'expand'=>'yes', 'fill'=>'x', 'pady'=>'2m')

msg = TkLabel.new(center) {
#  font $font
  wraplength '4i'
  justify 'left'
  text "これはメニューボタンのデモです。\"Below\"のボタンは\
下にメニューを出し、\"Right\"のボタンは右にメニューを出して、\
……となります。この文章の下には2つのオプションメニューがあります。\
1つは普通のメニューで、もう1つは16色のパレットです。"
}
msg.pack('side'=>'top', 'padx'=>25, 'pady'=>25)

TkFrame.new(center) {|f|
  menubuttonoptions = TkVariable.new
  mbutton = TkMenubutton.new(f)
  options = optionMenu(mbutton, menubuttonoptions,
                       'one', 'two', 'three')
  mbutton.pack('side'=>'left', 'padx'=>25, 'pady'=>25)
  paletteColor = TkVariable.new
  colors = ['Black','red4','DarkGreen','NavyBlue', 'gray75',
    'Red','Green','Blue','gray50','Yellow','Cyan','Magenta',
    'White','Brown','DarkSeaGreen','DarkViolet']
  colorMenuButton = TkMenubutton.new(f)
  m = optionMenu(colorMenuButton, paletteColor, *colors)
  begin
    windowingsystem = Tk.windowingsystem()
  rescue
    windowingsystem = ""
  end
  if windowingsystem == "classic" || windowingsystem == "aqua"
    topBorderColor = 'Black'
    bottomBorderColor = 'Black'
  else
    topBorderColor = 'gray50'
    bottomBorderColor = 'gray75'
  end
  for i in 0..15
    image = TkPhotoImage.new('height'=>16, 'width'=>16)
    image.put(topBorderColor, 0, 0, 16, 1)
    image.put(topBorderColor, 0, 1, 1, 16)
    image.put(bottomBorderColor, 0, 15, 16, 16)
    image.put(bottomBorderColor, 15, 1, 16, 16)
    image.put(colors[i], 1, 1, 15, 15)

    selectimage = TkPhotoImage.new('height'=>16, 'width'=>16)
    selectimage.put('Black', 0, 0, 16, 2)
    selectimage.put('Black', 0, 2, 2, 16)
    selectimage.put('Black', 2, 14, 16, 16)
    selectimage.put('Black', 14, 2, 16, 14)
    selectimage.put(colors[i], 2, 2, 14, 14)

    m.entryconfigure(i, 'image'=>image, 'selectimage'=>selectimage, 'hidemargin'=>'on')
  end
  m.configure('tearoff', 'on')
  for c in ['Black', 'gray75', 'gray50', 'White']
    m.entryconfigure(c, 'columnbreak'=>1)
  end
  colorMenuButton.pack('side'=>'left', 'padx'=>25, 'pady'=>25)
  pack 'padx'=>25, 'pady'=>25
}

end ; # Tk8.x
