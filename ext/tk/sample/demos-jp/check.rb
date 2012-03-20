# -*- coding: utf-8 -*-
#
# checkbutton widget demo (called by 'widget')
#

# toplevel widget が存在すれば削除する
if defined?($check_demo) && $check_demo
  $check_demo.destroy
  $check_demo = nil
end

# demo 用の toplevel widget を生成
$check_demo = TkToplevel.new {|w|
  title("Checkbutton Demonstration")
  iconname("check")
  positionWindow(w)
}

base_frame = TkFrame.new($check_demo).pack(:fill=>:both, :expand=>true)

# label 生成
msg = TkLabel.new(base_frame) {
  font $font
  wraplength '4i'
  justify 'left'
  text "下には 3 つのチェックボタンが表示されています。クリックするとボタンの選択状態が変わり、Tcl 変数 ( TkVariable オブジェクトでアクセスできます ) にそのボタンの状態を示す値を設定します。現在の変数の値を見るには「変数参照」ボタンをクリックしてください。"
}
msg.pack('side'=>'top')

# 変数生成
wipers = TkVariable.new(0)
brakes = TkVariable.new(0)
sober  = TkVariable.new(0)

# frame 生成
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    #text '了解'
    text '閉じる'
    command proc{
      tmppath = $check_demo
      $check_demo = nil
      $showVarsWin[tmppath.path] = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'コード参照'
    command proc{showCode 'check'}
  }.pack('side'=>'left', 'expand'=>'yes')


  TkButton.new(frame) {
    text '変数参照'
    command proc{
      showVars(base_frame,
               ['wipers', wipers], ['brakes', brakes], ['sober', sober])
    }
  }.pack('side'=>'left', 'expand'=>'yes')

}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')


# checkbutton 生成
[ TkCheckButton.new(base_frame, 'text'=>'ワイパー OK', 'variable'=>wipers),
  TkCheckButton.new(base_frame, 'text'=>'ブレーキ OK', 'variable'=>brakes),
  TkCheckButton.new(base_frame, 'text'=>'運転手 素面', 'variable'=>sober)
].each{|w| w.relief('flat'); w.pack('side'=>'top', 'pady'=>2, 'anchor'=>'w')}

