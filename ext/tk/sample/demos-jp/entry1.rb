# -*- coding: utf-8 -*-
#
# entry (no scrollbars) widget demo (called by 'widget')
#

# toplevel widget が存在すれば削除する
if defined?($entry1_demo) && $entry1_demo
  $entry1_demo.destroy
  $entry1_demo = nil
end

# demo 用の toplevel widget を生成
$entry1_demo = TkToplevel.new {|w|
  title("Entry Demonstration (no scrollbars)")
  iconname("entry1")
  positionWindow(w)
}

base_frame = TkFrame.new($entry1_demo).pack(:fill=>:both, :expand=>true)

# label 生成
msg = TkLabel.new(base_frame) {
  font $font
  wraplength '5i'
  justify 'left'
  text "3種類の異なるエントリが表示されています。文字を入力するにはポインタを持って行き、クリックしてからタイプしてください。標準的なMotifの編集機能が、Emacsのキーバインドとともに、サポートされています。例えば、バックスペースとコントロール-Hはカーソルの左の文字を削除し、デリートキーとコントロール-Dはカーソルの右側の文字を削除します。長過ぎてウィンドウに入り切らないものは、マウスのボタン2を押したままドラッグすることでスクロールさせることができます。日本語を入力するのはコントロール-バックスラッシュです。kinput2が動いていれば入力することができます。"
}
msg.pack('side'=>'top')

# frame 生成
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    #text '了解'
    text '閉じる'
    command proc{
      tmppath = $entry1_demo
      $entry1_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'コード参照'
    command proc{showCode 'entry1'}
  }.pack('side'=>'left', 'expand'=>'yes')
}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# entry 生成
e1 = TkEntry.new(base_frame, 'relief'=>'sunken')
e2 = TkEntry.new(base_frame, 'relief'=>'sunken')
e3 = TkEntry.new(base_frame, 'relief'=>'sunken')
[e1,e2,e3].each{|w| w.pack('side'=>'top', 'padx'=>10, 'pady'=>5, 'fill'=>'x')}

# 初期値挿入
e1.insert(0, '初期値')
e2.insert('end', "このエントリには長い文字列が入っていて、")
e2.insert('end', "長すぎてウィンドウには入り切らないので、")
e2.insert('end', "実際の所終りまで見るにはスクロールさせなければ")
e2.insert('end', "ならないでしょう。")

