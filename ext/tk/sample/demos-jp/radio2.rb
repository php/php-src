# -*- coding: utf-8 -*-
#
# radio2.rb
#
# This demonstration script creates a toplevel window containing
# several radiobutton widgets.
#
# radiobutton widget demo (called by 'widget')
#

# toplevel widget
if defined?($radio2_demo) && $radio2_demo
  $radio2_demo.destroy
  $radio2_demo = nil
end

# demo toplevel widget
$radio2_demo = TkToplevel.new {|w|
  title("Radiobutton Demonstration 2")
  iconname("radio2")
  positionWindow(w)
}

base_frame = TkFrame.new($radio2_demo).pack(:fill=>:both, :expand=>true)

# label
msg = TkLabel.new(base_frame) {
  font $font
  wraplength '5i'
  justify 'left'
  text "下には3つのラジオボタングループが表示されています。ボタンをクリックすると、そのボタンだけがそのグループの中で選択されます。各グループに対してそのグループの中のどのボタンが選択されているかを示す変数が割り当てられています。現在の変数の値を見るには「変数参照」ボタンをクリックしてください。"
}
msg.pack('side'=>'top')

#
size = TkVariable.new
color = TkVariable.new
align = TkVariable.new

# frame
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    #text '了解'
    text '閉じる'
    command proc{
      tmppath = $radio2_demo
      $radio2_demo = nil
      $showVarsWin[tmppath.path] = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'コード参照'
    command proc{showCode 'radio2'}
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text '変数参照'
    command proc{
      showVars(base_frame,
               ['size', size], ['color', color], ['compound', align])
    }
  }.pack('side'=>'left', 'expand'=>'yes')
}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# frame
f_left  = TkLabelFrame.new(base_frame, 'text'=>'文字サイズ',
                           'pady'=>2, 'padx'=>2)
f_mid   = TkLabelFrame.new(base_frame, 'text'=>'色',
                           'pady'=>2, 'padx'=>2)
f_right = TkLabelFrame.new(base_frame, 'text'=>'ビットマップ配置',
                           'pady'=>2, 'padx'=>2)
f_left.pack('side'=>'left', 'expand'=>'yes', 'padx'=>'.5c', 'pady'=>'.5c')
f_mid.pack('side'=>'left', 'expand'=>'yes', 'padx'=>'.5c', 'pady'=>'.5c')
f_right.pack('side'=>'left', 'expand'=>'yes', 'padx'=>'.5c', 'pady'=>'.5c')

# radiobutton
[10, 12, 18, 24].each {|sz|
  TkRadioButton.new(f_left) {
    text "ポイントサイズ #{sz}"
    variable size
    relief 'flat'
    value sz
  }.pack('side'=>'top', 'pady'=>2, 'anchor'=>'w', 'fill'=>'x')
}

['赤', '緑', '青', '黄', '橙', '紫'].each {|col|
  TkRadioButton.new(f_mid) {
    text col
    variable color
    relief 'flat'
    value col.downcase
    anchor 'w'
  }.pack('side'=>'top', 'pady'=>2, 'fill'=>'x')
}

# label = TkLabel.new(f_right, 'text'=>'ラベル', 'bitmap'=>'questhead',
label = Tk::Label.new(f_right, 'text'=>'ラベル', 'bitmap'=>'questhead',
                    'compound'=>'left')
label.configure('width'=>TkWinfo.reqwidth(label), 'compound'=>'top')
label.height(TkWinfo.reqheight(label))
abtn = ['Top', 'Left', 'Right', 'Bottom'].collect{|a|
  lower = a.downcase
  TkRadioButton.new(f_right, 'text'=>a, 'variable'=>align, 'relief'=>'flat',
                    'value'=>lower, 'indicatoron'=>0, 'width'=>7,
                    'command'=>proc{label.compound(align.value)})
}

Tk.grid('x', abtn[0])
Tk.grid(abtn[1], label, abtn[2])
Tk.grid('x', abtn[3])
