# -*- coding: utf-8 -*-
#
# radio3.rb
#
# This demonstration script creates a toplevel window containing
# several radiobutton widgets.
#
# radiobutton widget demo (called by 'widget')
#

# toplevel widget
if defined?($radio3_demo) && $radio3_demo
  $radio3_demo.destroy
  $radio3_demo = nil
end

# demo toplevel widget
$radio3_demo = TkToplevel.new {|w|
  title("Radiobutton Demonstration 3")
  iconname("radio3")
  positionWindow(w)
}

base_frame = TkFrame.new($radio3_demo).pack(:fill=>:both, :expand=>true)

# label
msg = TkLabel.new(base_frame) {
  font $font
  wraplength '5i'
  justify 'left'
  text '下には3つのラジオボタングループが表示されています。ボタンをクリックすると、そのグループに属するすべてのボタンの中でクリックしたボタンだけが選択された状態になります。各グループには、そのグループの中のどのボタンが選択されているかを示す変数が割り当てられています。「トライステート」ボタンが押されたときには、ラジオボタンの表示がトライステートモードになります。いずれかのボタンを選択すれば、ボタンの状態は元のように個々のボタンのon/off状態を示すようになります。現在の変数の値を見るには「変数参照」ボタンをクリックしてください。'
}
msg.grid(:row=>0, :column=>0, :columnspan=>3, :sticky=>'nsew')

# variable
size = TkVariable.new
color = TkVariable.new
align = TkVariable.new

# frame
TkFrame.new(base_frame) {|frame|
  TkGrid(TkFrame.new(frame, :height=>2, :relief=>:sunken, :bd=>2),
         :columnspan=>4, :row=>0, :sticky=>'ew', :pady=>2)
  TkGrid('x',
         TkButton.new(frame, :text=>'変数参照',
                      :image=>$image['view'], :compound=>:left,
                      :command=>proc{
                        showVars(base_frame, ['size', size],
                                 ['color', color], ['compound', align])
                      }),
         TkButton.new(frame, :text=>'コード参照',
                      :image=>$image['view'], :compound=>:left,
                      :command=>proc{showCode 'radio3'}),
         TkButton.new(frame, :text=>'閉じる',
                      :image=>$image['delete'], :compound=>:left,
                      :command=>proc{
                        tmppath = $radio3_demo
                        $radio3_demo = nil
                        $showVarsWin[tmppath.path] = nil
                        tmppath.destroy
                      }),
         :padx=>4, :pady=>4)
  frame.grid_columnconfigure(0, :weight=>1)
  TkGrid(frame, :row=>3, :column=>0, :columnspan=>3, :sticky=>'nsew')
}

# frame
f_left  = TkLabelFrame.new(base_frame, 'text'=>'文字サイズ',
                           'pady'=>2, 'padx'=>2)
f_mid   = TkLabelFrame.new(base_frame, 'text'=>'色',
                           'pady'=>2, 'padx'=>2)
f_right = TkLabelFrame.new(base_frame, 'text'=>'ビットマップ配置',
                           'pady'=>2, 'padx'=>2)
f_left .grid('column'=>0, 'row'=>1, 'pady'=>'.5c', 'padx'=>'.5c', 'rowspan'=>2)
f_mid  .grid('column'=>1, 'row'=>1, 'pady'=>'.5c', 'padx'=>'.5c', 'rowspan'=>2)
f_right.grid('column'=>2, 'row'=>1, 'pady'=>'.5c', 'padx'=>'.5c')

TkButton.new(base_frame, 'text'=>'トライステート',
             'command'=>proc{size.value = 'multi'; color.value = 'multi'}){
  grid('column'=>2, 'row'=>2, 'pady'=>'.5c', 'padx'=>'.5c')
}

# radiobutton
[10, 12, 14, 18, 24].each {|sz|
  TkRadioButton.new(f_left) {
    text "ポイントサイズ #{sz}"
    variable size
    relief 'flat'
    value sz
    tristatevalue 'multi'
  }.pack('side'=>'top', 'pady'=>2, 'anchor'=>'w', 'fill'=>'x')
}

['Red', 'Green', 'Blue', 'Yellow', 'Orange', 'Purple'].each {|col|
  TkRadioButton.new(f_mid) {
    text col
    variable color
    relief 'flat'
    value col.downcase
    anchor 'w'
    tristatevalue 'multi'
    command proc{f_mid.fg(color.value)}
  }.pack('side'=>'top', 'pady'=>2, 'fill'=>'x')
}

# label = TkLabel.new(f_right, 'text'=>'ラベル', 'bitmap'=>'questhead',
label = Tk::Label.new(f_right, 'text'=>'ラベル', 'bitmap'=>'questhead',
                    'compound'=>'left')
label.configure('width'=>TkWinfo.reqwidth(label), 'compound'=>'top')
label.height(TkWinfo.reqheight(label))
a_btn = ['Top', 'Left', 'Right', 'Bottom'].collect{|a|
  TkRadioButton.new(f_right, 'text'=>a, 'variable'=>align, 'relief'=>'flat',
                    'value'=>a.downcase, 'indicatoron'=>0, 'width'=>7,
                    'command'=>proc{label.compound(align.value)})
}

Tk.grid('x', a_btn[0])
Tk.grid(a_btn[1], label, a_btn[2])
Tk.grid('x', a_btn[3])
