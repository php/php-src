# -*- coding: utf-8 -*-
#
# button widget demo (called by 'widget')
#
#

# toplevel widget が存在すれば削除する
if defined?($button_demo) && $button_demo
  $button_demo.destroy
  $button_demo = nil
end

# demo 用の toplevel widget を生成
$button_demo = TkToplevel.new {|w|
  title("Button Demonstration")
  iconname("button")
  positionWindow(w)
}

# label 生成
msg = TkLabel.new($button_demo) {
  font $kanji_font
  wraplength '4i'
  justify 'left'
  text "ボタンをクリックすると、ボタンの背景色がそのボタンに書かれている色になります。ボタンからボタンへの移動はタブを押すことでも可能です。またスペースで実行することができます。"
}
msg.pack('side'=>'top')

# frame 生成
$button_buttons = Tk::Frame.new($button_demo) {|frame|
  TkButton.new(frame) {
    #text '了解'
    text '閉じる'
    command proc{
      tmppath = $button_demo
      $button_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'コード参照'
    command proc{showCode 'button'}
  }.pack('side'=>'left', 'expand'=>'yes')

}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# button 生成
TkButton.new($button_demo){
  text "Peach Puff"
  width 10
  command proc{
    $button_demo.configure('bg','PeachPuff1')
    $button_buttons.configure('bg','PeachPuff1')
  }
}.pack('side'=>'top', 'expand'=>'yes', 'pady'=>2)

TkButton.new($button_demo){
  text "Light Blue"
  width 10
  command proc{
    $button_demo.configure('bg','LightBlue1')
    $button_buttons.configure('bg','LightBlue1')
  }
}.pack('side'=>'top', 'expand'=>'yes', 'pady'=>2)

TkButton.new($button_demo){
  text "Sea Green"
  width 10
  command proc{
    $button_demo.configure('bg','SeaGreen2')
    $button_buttons.configure('bg','SeaGreen2')
  }
}.pack('side'=>'top', 'expand'=>'yes', 'pady'=>2)

TkButton.new($button_demo){
  text "Yellow"
  width 10
  command proc{
    $button_demo.configure('bg','Yellow1')
    $button_buttons.configure('bg','Yellow1')
  }
}.pack('side'=>'top', 'expand'=>'yes', 'pady'=>2)
