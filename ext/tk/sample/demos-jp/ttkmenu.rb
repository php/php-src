# -*- coding: utf-8 -*-
#
# ttkmenu.rb --
#
# This demonstration script creates a toplevel window containing several Ttk
# menubutton widgets.
#
# based on "Id: ttkmenu.tcl,v 1.3 2007/12/13 15:27:07 dgp Exp"

if defined?($ttkmenu_demo) && $ttkmenu_demo
  $ttkmenu_demo.destroy
  $ttkmenu_demo = nil
end

$ttkmenu_demo = TkToplevel.new {|w|
  title("Ttk Menu Buttons")
  iconname("ttkmenu")
  positionWindow(w)
}

base_frame = Ttk::Frame.new($ttkmenu_demo).pack(:fill=>:both, :expand=>true)

Ttk::Label.new(base_frame, :font=>$font, :wraplength=>'4i', :justify=>:left,
               :text=><<EOL).pack(:side=>:top, :fill=>:x)
Ttkとは，テーマ指定可能な新しいウィジェット集合です．\
これによりテーマに対応することができるようになったウィジェットのひとつに\
メニューボタンがあります．\
以下では，テーマに対応したメニューボタンがいくつか表示されています．\
それらを使って，現在使用中のテーマを変更することが可能です．\
テーマの選択がメニューボタン自身の見掛けを変化させる様子や，\
中央のメニューボタンだけが異なるスタイル\
(ツールバーでの一般的な表示に適したもの)で表示されている様子に\
注目してください．\
なお，メニューボタンについてはテーマに対応したウィジェットがありますが，\
メニューについてはテーマに対応したウィジェットは含まれていません．\
その理由は，標準のTkのメニューウィジェットが\
すべてのプラットホームで十分に良好な見掛けと操作性を持っている，\
特に，多くの環境でその環境本来の操作体系となるように実装されていると\
判断されたことによります．
EOL

Ttk::Separator.new(base_frame).pack(:side=>:top, :fill=>:x)

## See Code / Dismiss
Ttk::Frame.new($ttkmenu_demo) {|frame|
  sep = Ttk::Separator.new(frame)
  Tk.grid(sep, :columnspan=>4, :row=>0, :sticky=>'ew', :pady=>2)
  TkGrid('x',
         Ttk::Button.new(frame, :text=>'コード参照',
                         :image=>$image['view'], :compound=>:left,
                         :command=>proc{showCode 'ttkmenu'}),
         Ttk::Button.new(frame, :text=>'閉じる',
                         :image=>$image['delete'], :compound=>:left,
                         :command=>proc{
                           $ttkmenu_demo.destroy
                           $ttkmenu_demo = nil
                         }),
         :padx=>4, :pady=>4)
  grid_columnconfigure(0, :weight=>1)
  pack(:side=>:bottom, :fill=>:x)
}

b1 = Ttk::Menubutton.new(base_frame,:text=>'テーマを選択',:direction=>:above)
b2 = Ttk::Menubutton.new(base_frame,:text=>'テーマを選択',:direction=>:left)
b3 = Ttk::Menubutton.new(base_frame,:text=>'テーマを選択',:direction=>:right)
b4 = Ttk::Menubutton.new(base_frame,:text=>'テーマを選択',:direction=>:flush,
                         :style=>Ttk::Menubutton.style('Toolbutton'))
b5 = Ttk::Menubutton.new(base_frame,:text=>'テーマを選択',:direction=>:below)

b1.menu(m1 = Tk::Menu.new(b1, :tearoff=>false))
b2.menu(m2 = Tk::Menu.new(b2, :tearoff=>false))
b3.menu(m3 = Tk::Menu.new(b3, :tearoff=>false))
b4.menu(m4 = Tk::Menu.new(b4, :tearoff=>false))
b5.menu(m5 = Tk::Menu.new(b5, :tearoff=>false))

Ttk.themes.each{|theme|
  m1.add(:command, :label=>theme, :command=>proc{Ttk.set_theme theme})
  m2.add(:command, :label=>theme, :command=>proc{Ttk.set_theme theme})
  m3.add(:command, :label=>theme, :command=>proc{Ttk.set_theme theme})
  m4.add(:command, :label=>theme, :command=>proc{Ttk.set_theme theme})
  m5.add(:command, :label=>theme, :command=>proc{Ttk.set_theme theme})
}

f = Ttk::Frame.new(base_frame).pack(:fill=>:x)
f1 = Ttk::Frame.new(base_frame).pack(:fill=>:both, :expand=>true)
f.lower

f.grid_anchor(:center)
TkGrid('x', b1, 'x', :in=>f, :padx=>3, :pady=>2)
TkGrid(b2,  b4, b3,  :in=>f, :padx=>3, :pady=>2)
TkGrid('x', b5, 'x', :in=>f, :padx=>3, :pady=>2)
