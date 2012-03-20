# -*- coding: utf-8 -*-
#
# spin.rb --
#
# This demonstration script creates several spinbox widgets.
#
# based on Tcl/Tk8.4.4 widget demos

if defined?($spin_demo) && $spin_demo
  $spin_demo.destroy
  $spin_demo = nil
end

$spin_demo = TkToplevel.new {|w|
  title("Spinbox Demonstration")
  iconname("spin")
  positionWindow(w)
}

base_frame = TkFrame.new($spin_demo).pack(:fill=>:both, :expand=>true)

TkLabel.new(base_frame,
            :font=>$font, :wraplength=>'5i', :justify=>:left,
            :text=><<EOL).pack(:side=>:top)
下には３種類のスピンボックスが表示されています。
それぞれ、マウスで選択して文字を入力することができます。
編集操作としては、Emacs 形式の多くに加えて、一般的な
Motif 形式のキー操作がサポートされています。たとえば、
Backspace と Control-h とは入力カーソルの左側の文字を
削除し、Delete と Control-d とは右側の文字を削除します。
入力枠の長さを越えるような長い文字列を入力した場合には、
マウスのボタン２を押してドラッグすることで、入力文字列
をスキャンすることが可能です。
なお、最初のスピンボックスは、整数値とみなされるような
文字列しか入力が許されないことに注意してください。また、
３番目のスピンボックスで選択候補に現れるのはオーストラ
リアの都市名のリストとなっています。
もしあなたが使っている Ruby にリンクされている Tk ライ
ブラリが spinbox ウィジェットを実装していない場合、この
デモはうまく動かないはずです。その場合には spinbox ウィ
ジェットが実装されているようなより新しいバージョンの Tk
を組み合わせて試すようにしてください。
EOL

TkFrame.new(base_frame){|f|
  pack(:side=>:bottom, :fill=>:x, :pady=>'2m')

  TkButton.new(f, :text=>'閉じる', :width=>15, :command=>proc{
                 $spin_demo.destroy
                 $spin_demo = nil
               }).pack(:side=>:left, :expand=>true)

  TkButton.new(f, :text=>'コード参照', :width=>15, :command=>proc{
                 showCode 'spin'
               }).pack(:side=>:left, :expand=>true)
}

australianCities = [
    'Canberra', 'Sydney', 'Melbourne', 'Perth', 'Adelaide',
    'Brisbane', 'Hobart', 'Darwin', 'Alice Springs'
]

[
  TkSpinbox.new(base_frame, :from=>1, :to=>10, :width=>10, :validate=>:key,
                :validatecommand=>[
                  proc{|s| s == '' || /^[+-]?\d+$/ =~ s }, '%P'
                ]),
  TkSpinbox.new(base_frame, :from=>0, :to=>3, :increment=>0.5,
                :format=>'%05.2f', :width=>10),
  TkSpinbox.new(base_frame, :values=>australianCities, :width=>10)
].each{|sbox| sbox.pack(:side=>:top, :pady=>5, :padx=>10)}
