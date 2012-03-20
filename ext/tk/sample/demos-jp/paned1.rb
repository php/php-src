# -*- coding: utf-8 -*-
#
# paned1.rb
#
# This demonstration script creates a toplevel window containing
# a paned window that separates two windows horizontally.
#
# based on "Id: paned1.tcl,v 1.1 2002/02/22 14:07:01 dkf Exp"

if defined?($paned1_demo) && $paned1_demo
  $paned1_demo.destroy
  $paned1_demo = nil
end

$paned1_demo = TkToplevel.new {|w|
  title("Horizontal Paned Window Demonstration")
  iconname("paned1")
  positionWindow(w)
}

base_frame = TkFrame.new($paned1_demo).pack(:fill=>:both, :expand=>true)

TkLabel.new(base_frame,
            :font=>$font, :wraplength=>'4i', :justify=>:left,
            :text=><<EOL).pack(:side=>:top)
下の色付けされた二つのウィンドウの間の仕切り枠は、一つの領域をそれぞれのウィンドウのために分割するためのものです。左ボタンで仕切りを操作すると、分割サイズ変更の操作途中では再表示はなされず、確定させたときに表示が更新されます。マウスによる仕切りの操作に追随してサイズを変更した表示がなわれるようにしたい場合は、マウスの中央ボタンを使ってください。
もしあなたが使っている Ruby にリンクされている Tk ライブラリが panedwindow を実装していない
場合、このデモはうまく動かないはずです。その場合には panedwindow が実装されているような
より新しいバージョンの Tk を組み合わせて試す
ようにしてください。
EOL

# The bottom buttons
TkFrame.new(base_frame){|f|
  pack(:side=>:bottom, :fill=>:x, :pady=>'2m')

  TkButton.new(f, :text=>'閉じる', :width=>15, :command=>proc{
                 $paned1_demo.destroy
                 $paned1_demo = nil
               }).pack(:side=>:left, :expand=>true)

  TkButton.new(f, :text=>'コード参照', :width=>15, :command=>proc{
                 showCode 'paned1'
               }).pack(:side=>:left, :expand=>true)
}

TkPanedwindow.new(base_frame, :orient=>:horizontal){|f|
  add(Tk::Label.new(f, :text=>"This is the\nleft side", :bg=>'yellow'),
      Tk::Label.new(f, :text=>"This is the\nright side", :bg=>'cyan'))

  pack(:side=>:top, :expand=>true, :fill=>:both, :pady=>2, :padx=>'2m')
}
