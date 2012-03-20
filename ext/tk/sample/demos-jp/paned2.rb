# -*- coding: utf-8 -*-
#
# paned2.rb --
#
# This demonstration script creates a toplevel window containing
# a paned window that separates two windows vertically.
#
# based on "Id: paned2.tcl,v 1.1 2002/02/22 14:07:01 dkf Exp"

if defined?($paned2_demo) && $paned2_demo
  $paned2_demo.destroy
  $paned2_demo = nil
end

$paned2_demo = TkToplevel.new {|w|
  title("Vertical Paned Window Demonstration")
  iconname("paned2")
  positionWindow(w)
}

base_frame = TkFrame.new($paned2_demo).pack(:fill=>:both, :expand=>true)

TkLabel.new(base_frame,
            :font=>$font, :wraplength=>'4i', :justify=>:left,
            :text=><<EOL).pack(:side=>:top)
下のスクロールバー付きのウィジェットが置かれた二つのウィンドウの間の仕切り枠は、一つの領域をそれぞれのウィンドウのために分割するためのものです。左ボタンで仕切りを操作すると、分割サイズ変更の操作途中では再表示はなされず、確定させたときに表示が更新されます。マウスによる仕切りの操作に追随してサイズを変更した表示がなわれるようにしたい場合は、マウスの中央ボタンを使ってください。
もしあなたが使っている Ruby にリンクされている Tk ライブラリが panedwindow を実装していない
場合、このデモはうまく動かないはずです。その場合には panedwindow が実装されているような
より新しいバージョンの Tk を組み合わせて試す
ようにしてください。
EOL

# The bottom buttons
TkFrame.new(base_frame){|f|
  pack(:side=>:bottom, :fill=>:x, :pady=>'2m')

  TkButton.new(f, :text=>'閉じる', :width=>15, :command=>proc{
                 $paned2_demo.destroy
                 $paned2_demo = nil
               }).pack(:side=>:left, :expand=>true)

  TkButton.new(f, :text=>'コード参照', :width=>15, :command=>proc{
                 showCode 'paned2'
               }).pack(:side=>:left, :expand=>true)
}

paneList = TkVariable.new  # define as normal variable (not array)
paneList.value = [         # ruby's array --> tcl's list
    'Ruby/Tk のウィジェット一覧',
    'TkButton',
    'TkCanvas',
    'TkCheckbutton',
    'TkEntry',
    'TkFrame',
    'TkLabel',
    'TkLabelframe',
    'TkListbox',
    'TkMenu',
    'TkMenubutton',
    'TkMessage',
    'TkPanedwindow',
    'TkRadiobutton',
    'TkScale',
    'TkScrollbar',
    'TkSpinbox',
    'TkText',
    'TkToplevel'
]

# Create the pane itself
TkPanedwindow.new(base_frame, :orient=>:vertical){|f|
  pack(:side=>:top, :expand=>true, :fill=>:both, :pady=>2, :padx=>'2m')

  add(TkFrame.new(f){|paned2_top|
        TkListbox.new(paned2_top, :listvariable=>paneList) {
          # Invert the first item to highlight it
          itemconfigure(0, :background=>self.cget(:foreground),
                           :foreground=>self.cget(:background) )
          yscrollbar(TkScrollbar.new(paned2_top).pack(:side=>:right,
                                                      :fill=>:y))
          pack(:fill=>:both, :expand=>true)
        }
      },

      TkFrame.new(f, :height=>120) {|paned2_bottom|
        # The bottom window is a text widget with scrollbar
        paned2_xscr = TkScrollbar.new(paned2_bottom)
        paned2_yscr = TkScrollbar.new(paned2_bottom)
        paned2_text = TkText.new(paned2_bottom, :width=>30, :wrap=>:non) {
          insert('1.0', 'ここに配置されているのは、' +
                        'ごく普通のテキストウィジェットです。')
          xscrollbar(paned2_xscr)
          yscrollbar(paned2_yscr)
        }
        Tk.grid(paned2_text, paned2_yscr, :sticky=>'nsew')
        Tk.grid(paned2_xscr, :sticky=>'nsew')
        TkGrid.columnconfigure(paned2_bottom, 0, :weight=>1)
        TkGrid.rowconfigure(paned2_bottom, 0, :weight=>1)
      } )
}
