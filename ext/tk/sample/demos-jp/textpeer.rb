# -*- coding: utf-8 -*-
#
# text widget peering demo (called by 'widget')
#
# based on Tcl/Tk8.5.0 widget demos

if defined?($textpeer_demo) && $textpeer_demo
  $textpeer_demo.destroy
  $textpeer_demo = nil
end

# demo toplevel widget
$textpeer_demo = TkToplevel.new {|w|
  title("Text Wdget Peering Demonstration")
  iconname("textpeer")
  positionWindow(w)
}

base_frame = TkFrame.new($textpeer_demo).pack(:fill=>:both, :expand=>true)

count = [0]

## Define a widget that we peer from; it won't ever actually be shown though
first = TkText.new(base_frame, :widgetname=>"text#{count[0] += 1}")
first.insert :end,"このデモは一つの組を成したテキストウィジェットを示します。"
first.insert :end,"それらのテキストウィジェットは対等(ピア;peer)の関係に"
first.insert :end,"なっています。"
first.insert :end,"それらは、基盤となるデータモデルは共通のものを持ちますが、"
first.insert :end,"画面表示位置、編集位置、選択範囲(selection)については"
first.insert :end,"独立に持つことができます。"
first.insert :end,"各テキストウィジェットの脇にある"
first.insert :end,"「ピア(peer)の作成」ボタンを使えば、"
first.insert :end,"新たなピアを追加することが可能です。"
first.insert :end,"また「ピア(peer)の消去」ボタンを使えば、"
first.insert :end,"特定のピアウィジェットを消去することもできます。"

Tk.update_idletasks  ## for 'first' widget

## Procedures to make and kill clones; most of this is just so that the demo
## looks nice...
def makeClone(count, win, txt)
  cnt = (count[0] += 1)
  peer = TkText::Peer.new(txt, win, :widgetname=>"text#{cnt}")
  sbar = TkScrollbar.new(win, :widgetname=>"sb#{cnt}")
  peer.yscrollbar sbar
  b1 = TkButton.new(win, :widgetname=>"clone#{cnt}",
                    :text=>'ピア(peer)の作成',
                    :command=>proc{makeClone(count, win, peer)})
  b2 = TkButton.new(win, :widgetname=>"kill#{cnt}",
                    :text=>'ピア(peer)の消去',
                    :command=>proc{killClone(win, cnt)})
  row = cnt * 2
  TkGrid.configure(peer, sbar, b1, :sticky=>'nsew', :row=>row)
  TkGrid.configure('^',  '^',  b2, :sticky=>'nsew', :row=>(row+=1))
  TkGrid.configure(b1,  b2, :sticky=>'new')
  TkGrid.rowconfigure(win,  b2, :weight=>1)
end

def killClone(win, cnt)
  Tk.destroy("#{win.path}.text#{cnt}",  "#{win.path}.sb#{cnt}",
             "#{win.path}.clone#{cnt}", "#{win.path}.kill#{cnt}")
end

## Now set up the GUI
makeClone(count, base_frame, first)
makeClone(count, base_frame, first)
first.destroy

## See Code / Dismiss buttons
TkFrame.new(base_frame){|f|
  TkButton.new(f, :text=>'閉じる', :width=>15, :command=>proc{
                 $textpeer_demo.destroy
                 $textpeer_demo = nil
               }).pack(:side=>:left, :expand=>true)

  TkButton.new(f, :text=>'コード参照', :width=>15, :command=>proc{
                 showCode 'textpeer'
               }).pack(:side=>:left, :expand=>true)

  TkGrid.configure(f, '-', '-', :sticky=>'ew', :row=>5000)
}
TkGrid.columnconfigure(base_frame, 0, :weight=>1)
