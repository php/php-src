# -*- coding: utf-8 -*-
#
# label widget demo (called by 'widget')
#

# toplevel widget が存在すれば削除する
if defined?($label_demo) && $label_demo
  $label_demo.destroy
  $label_demo = nil
end

# demo 用の toplevel widget を生成
$label_demo = TkToplevel.new {|w|
  title("Label Demonstration")
  iconname("label")
  positionWindow(w)
}

base_frame = TkFrame.new($label_demo).pack(:fill=>:both, :expand=>true)

# label 生成
msg = TkLabel.new(base_frame) {
  font $font
  wraplength '4i'
  justify 'left'
  text "下には5つのラベルが表示されています。左側にはテキストラベルが3つあり、右側にはビットマップラベルとテキストラベルがあります。ラベルというのはあまり面白いものではありません。なぜなら眺める以外何もできないからです。"
}
msg.pack('side'=>'top')

# frame 生成
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    #text '了解'
    text '閉じる'
    command proc{
      tmppath = $label_demo
      $label_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'コード参照'
    command proc{showCode 'label'}
  }.pack('side'=>'left', 'expand'=>'yes')

}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# label demo 用フレーム生成
f_left = TkFrame.new(base_frame)
f_right = TkFrame.new(base_frame)
[f_left, f_right].each{|w| w.pack('side'=>'left', 'expand'=>'yes',
                                  'padx'=>10, 'pady'=>10, 'fill'=>'both')}

# label 生成
[ TkLabel.new(f_left, 'text'=>'最初のラベル'),
  TkLabel.new(f_left, 'text'=>'2 番目。ちょっと浮き上がらせてみました',
              'relief'=>'raised'),
  TkLabel.new(f_left, 'text'=>'3 番目。沈んでいます ', 'relief'=>'sunken')
].each{|w| w.pack('side'=>'top', 'expand'=>'yes', 'pady'=>2, 'anchor'=>'w')}

# TkLabel.new(f_right) {
Tk::Label.new(f_right) {
  bitmap('@' + [$demo_dir,'..','images','face.xbm'].join(File::Separator))
  borderwidth 2
  relief 'sunken'
}.pack('side'=>'top')

TkLabel.new(f_right) { text 'Tcl/Tk 所有者' }.pack('side'=>'top')
