# -*- coding: utf-8 -*-
#
# listbox widget demo 'states' (called by 'widget')
#

# toplevel widget が存在すれば削除する
if defined?($states_demo) && $states_demo
  $states_demo.destroy
  $states_demo = nil
end

# demo 用の toplevel widget を生成
$states_demo = TkToplevel.new {|w|
  title("Listbox Demonstration (states)")
  iconname("states")
  positionWindow(w)
}

base_frame = TkFrame.new($states_demo).pack(:fill=>:both, :expand=>true)

# label 生成
msg = TkLabel.new(base_frame) {
  font $font
  wraplength '4i'
  justify 'left'
  text "下にあるのは都道府県名が入ったスクロールバー付のリストボックスです。リストをスクロールさせるのはスクロールバーでもできますし、リストボックスの中でマウスのボタン2(中ボタン)を押したままドラッグしてもできます。"
}
msg.pack('side'=>'top')

# frame 生成
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    #text '了解'
    text '閉じる'
    command proc{
      tmppath = $states_demo
      $states_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'コード参照'
    command proc{showCode 'states'}
  }.pack('side'=>'left', 'expand'=>'yes')

}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# frame 生成
states_lbox = nil
TkFrame.new(base_frame, 'borderwidth'=>'.5c') {|w|
  s = TkScrollbar.new(w)
  states_lbox = TkListbox.new(w) {
    setgrid 1
    height 12
    yscrollcommand proc{|first,last| s.set first,last}
  }
  s.command(proc{|*args| states_lbox.yview(*args)})
  s.pack('side'=>'right', 'fill'=>'y')
  states_lbox.pack('side'=>'left', 'expand'=>1, 'fill'=>'both')
}.pack('side'=>'top', 'expand'=>'yes', 'fill'=>'y')

ins_data = [
  '愛知','青森','秋田','石川','茨城','岩手','愛媛',
  '大分','大阪','岡山','沖縄','香川','鹿児島','神奈川',
  '岐阜','京都','熊本','群馬','高知','埼玉','佐賀',
  '滋賀','静岡','島根','千葉','東京','徳島','栃木',
  '鳥取','富山','長崎','長野','奈良','新潟','兵庫',
  '広島','福井','福岡','福島','北海道','三重','宮城',
  '宮崎','山形','山口','山梨','和歌山'
]

states_lbox.insert(0, *ins_data)

