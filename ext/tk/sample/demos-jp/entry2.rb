# -*- coding: utf-8 -*-
#
# entry (with scrollbars) widget demo (called by 'widget')
#

# toplevel widget が存在すれば削除する
if defined?($entry2_demo) && $entry2_demo
  $entry2_demo.destroy
  $entry2_demo = nil
end

# demo 用の toplevel widget を生成
$entry2_demo = TkToplevel.new {|w|
  title("Entry Demonstration (with scrollbars)")
  iconname("entry2")
  positionWindow(w)
}

base_frame = TkFrame.new($entry2_demo).pack(:fill=>:both, :expand=>true)

# label 生成
msg = TkLabel.new(base_frame) {
  font $font
  wraplength '5i'
  justify 'left'
  text "3種類の異なるエントリが各々スクロールバー付で表示されています。文字を入力するにはポインタを持って行き、クリックしてからタイプしてください。標準的なMotifの編集機能が、Emacsのキーバインドとともに、サポートされています。例えば、バックスペースとコントロール-Hはカーソルの左の文字を削除し、デリートキーとコントロール-Dはカーソルの右側の文字を削除します。長過ぎてウィンドウに入り切らないものは、マウスのボタン2を押したままドラッグすることでスクロールさせることができます。日本語を入力するのはコントロール-バックスラッシュです。kinput2が動いていれば入力することができます。"
}
msg.pack('side'=>'top')

# frame 生成
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    #text '了解'
    text '閉じる'
    command proc{
      tmppath = $entry2_demo
      $entry2_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'コード参照'
    command proc{showCode 'entry2'}
  }.pack('side'=>'left', 'expand'=>'yes')
}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# frame 生成
TkFrame.new(base_frame, 'borderwidth'=>10) {|w|
  # entry 1
  s1 = TkScrollbar.new(w, 'relief'=>'sunken', 'orient'=>'horiz')
  e1 = TkEntry.new(w, 'relief'=>'sunken') {
    xscrollcommand proc{|first,last| s1.set first,last}
  }
  s1.command(proc{|*args| e1.xview(*args)})
  e1.pack('side'=>'top', 'fill'=>'x')
  s1.pack('side'=>'top', 'fill'=>'x')

  # spacer
  TkFrame.new(w, 'width'=>20, 'height'=>10).pack('side'=>'top', 'fill'=>'x')

  # entry 2
  s2 = TkScrollbar.new(w, 'relief'=>'sunken', 'orient'=>'horiz')
  e2 = TkEntry.new(w, 'relief'=>'sunken') {
    xscrollcommand proc{|first,last| s2.set first,last}
  }
  s2.command(proc{|*args| e2.xview(*args)})
  e2.pack('side'=>'top', 'fill'=>'x')
  s2.pack('side'=>'top', 'fill'=>'x')

  # spacer
  TkFrame.new(w, 'width'=>20, 'height'=>10).pack('side'=>'top', 'fill'=>'x')

  # entry 3
  s3 = TkScrollbar.new(w, 'relief'=>'sunken', 'orient'=>'horiz')
  e3 = TkEntry.new(w, 'relief'=>'sunken') {
    xscrollcommand proc{|first,last| s3.set first,last}
  }
  s3.command(proc{|*args| e3.xview(*args)})
  e3.pack('side'=>'top', 'fill'=>'x')
  s3.pack('side'=>'top', 'fill'=>'x')

  # 初期値挿入
  e1.insert(0, '初期値')
  e2.insert('end', "このエントリには長い文字列が入っていて、")
  e2.insert('end', "長すぎてウィンドウには入り切らないので、")
  e2.insert('end', "実際の所終りまで見るにはスクロールさせなければ")
  e2.insert('end', "ならないでしょう。")

}.pack('side'=>'top', 'fill'=>'x', 'expand'=>'yes')

