# -*- coding: utf-8 -*-
#
# text (display styles) widget demo (called by 'widget')
#

# toplevel widget が存在すれば削除する
if defined?($style_demo) && $style_demo
  $style_demo.destroy
  $style_demo = nil
end


# demo 用の toplevel widget を生成
$style_demo = TkToplevel.new {|w|
  title("Text Demonstration - Display Styles")
  iconname("style")
  positionWindow(w)
}

base_frame = TkFrame.new($style_demo).pack(:fill=>:both, :expand=>true)

# frame 生成
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    #text '了解'
    text '閉じる'
    command proc{
      tmppath = $style_demo
      $style_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'コード参照'
    command proc{showCode 'style'}
  }.pack('side'=>'left', 'expand'=>'yes')
}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')


# text 生成
txt = TkText.new(base_frame){|t|
  # 生成
  setgrid 'true'
  #width  70
  #height 32
  wrap 'word'
  font $font
  TkScrollbar.new(base_frame) {|s|
    pack('side'=>'right', 'fill'=>'y')
    command proc{|*args| t.yview(*args)}
    t.yscrollcommand proc{|first,last| s.set first,last}
  }
  pack('expand'=>'yes', 'fill'=>'both')

  # テキストタグ設定 (フォント関連)
  family = 'Courier'

  if $tk_version =~ /^4.*/
    style_tag_bold = TkTextTag.new(t, 'font'=>'-*-Courier-Bold-O-Normal--*-120-*-*-*-*-*-*')
    style_tag_big = TkTextTag.new(t, 'font'=>'-*-Courier-Bold-R-Normal--*-140-*-*-*-*-*-*', 'kanjifont'=>$msg_kanji_font)
    style_tag_verybig = TkTextTag.new(t, 'font'=>'-*-Helvetica-Bold-R-Normal--*-240-*-*-*-*-*-*')
    # style_tag_small = TkTextTag.new(t, 'font'=>'-Adobe-Helvetica-Bold-R-Normal-*-100-*', 'kanjifont'=>$kanji_font)
    style_tag_small = TkTextTag.new(t, 'font'=>'-Adobe-Helvetica-Bold-R-Normal-*-100-*')
  else
    style_tag_bold = TkTextTag.new(t, 'font'=>[family, 12, :bold, :italic])
    style_tag_big = TkTextTag.new(t, 'font'=>[family, 14, :bold])
    style_tag_verybig = TkTextTag.new(t, 'font'=>['Helvetica', 24, :bold])
    style_tag_small = TkTextTag.new(t, 'font'=>'Times 8 bold')
  end
###
#  case($tk_version)
#  when /^4.*/
#    style_tag_big = TkTextTag.new(t, 'font'=>'-*-Courier-Bold-R-Normal--*-140-*-*-*-*-*-*', 'kanjifont'=>$msg_kanji_font)
#    style_tag_small = TkTextTag.new(t, 'font'=>'-Adobe-Helvetica-Bold-R-Normal-*-100-*', 'kanjifont'=>$kanji_font)
#  when /^8.*/
#    unless $style_demo_do_first
#      $style_demo_do_first = true
#      Tk.tk_call('font', 'create', '@bigascii',
#                '-copy', '-*-Courier-Bold-R-Normal--*-140-*-*-*-*-*-*')
#      Tk.tk_call('font', 'create', '@smallascii',
#                '-copy', '-Adobe-Helvetica-Bold-R-Normal-*-100-*')
#      Tk.tk_call('font', 'create', '@cBigFont',
#                '-compound', '@bigascii @msg_knj')
#      Tk.tk_call('font', 'create', '@cSmallFont',
#                '-compound', '@smallascii @kanji')
#    end
#    style_tag_big = TkTextTag.new(t, 'font'=>'@cBigFont')
#    style_tag_small = TkTextTag.new(t, 'font'=>'@cSmallFont')
#  end

  # テキストタグ設定 (色，レリーフ関連)
  if TkWinfo.depth($root).to_i > 1
    style_tag_color1 = TkTextTag.new(t, 'background'=>'#a0b7ce')
    style_tag_color2 = TkTextTag.new(t, 'foreground'=>'red')
    style_tag_raised = TkTextTag.new(t, 'relief'=>'raised', 'borderwidth'=>1)
    style_tag_sunken = TkTextTag.new(t, 'relief'=>'sunken', 'borderwidth'=>1)
  else
    style_tag_color1 = TkTextTag.new(t, 'background'=>'black',
                                     'foreground'=>'white')
    style_tag_color2 = TkTextTag.new(t, 'background'=>'black',
                                     'foreground'=>'white')
    style_tag_raised = TkTextTag.new(t, 'background'=>'white',
                                     'relief'=>'raised', 'borderwidth'=>1)
    style_tag_sunken = TkTextTag.new(t, 'background'=>'white',
                                     'relief'=>'sunken', 'borderwidth'=>1)
  end

  # テキストタグ設定 (その他)
  if $tk_version =~ /^4\.[01]/
    style_tag_bgstipple = TkTextTag.new(t, 'background'=>'black',
                                        'borderwidth'=>0,
                                        'bgstipple'=>'gray25')
  else
    style_tag_bgstipple = TkTextTag.new(t, 'background'=>'black',
                                        'borderwidth'=>0,
                                        'bgstipple'=>'gray12')
  end
  style_tag_fgstipple = TkTextTag.new(t, 'fgstipple'=>'gray50')
  style_tag_underline = TkTextTag.new(t, 'underline'=>'on')
  style_tag_overstrike = TkTextTag.new(t, 'overstrike'=>'on')
  style_tag_right  = TkTextTag.new(t, 'justify'=>'right')
  style_tag_center = TkTextTag.new(t, 'justify'=>'center')
  if $tk_version =~ /^4.*/
    style_tag_super = TkTextTag.new(t, 'offset'=>'4p', 'font'=>'-Adobe-Courier-Medium-R-Normal--*-100-*-*-*-*-*-*')
    style_tag_sub = TkTextTag.new(t, 'offset'=>'-2p', 'font'=>'-Adobe-Courier-Medium-R-Normal--*-100-*-*-*-*-*-*')
  else
    style_tag_super = TkTextTag.new(t, 'offset'=>'4p', 'font'=>[family, 10])
    style_tag_sub = TkTextTag.new(t, 'offset'=>'-2p', 'font'=>[family, 10])
  end
  style_tag_margins = TkTextTag.new(t, 'lmargin1'=>'12m', 'lmargin2'=>'6m',
                                    'rmargin'=>'10m')
  style_tag_spacing = TkTextTag.new(t, 'spacing1'=>'10p', 'spacing2'=>'2p',
                                    'lmargin1'=>'12m', 'lmargin2'=>'6m',
                                    'rmargin'=>'10m')

  # テキスト挿入
  insert('end', 'このようにテキスト widget は情報を様々なスタイルで表示すること
ができます。')
  insert('end', 'タグ', style_tag_big)
  insert('end', 'というメカニズムでコントロールされます。
タグとはテキスト widget 内のある文字 (の範囲)に対して適用できる
単なる名前のことです。タグは様々な表示スタイルに設定できます。
設定すると、そのタグのついた文字は指定したスタイルで表示される
ようになります。使用できる表示スタイルは次の通りです。
')
  insert('end', '
1. フォント', style_tag_big)
  insert('end', '    どんな X のフォントでも使えます。')
  insert('end', 'large', style_tag_verybig)
  insert('end', '
とか')
#  insert('end', '小さい', style_tag_small)
  insert('end', 'small', style_tag_small)
  insert('end', 'とか。
')
  insert('end', '
2. 色', style_tag_big)
  insert('end', '  ')
  insert('end', '背景色', style_tag_color1)
  insert('end', 'も')
  insert('end', '前景色', style_tag_color2)
  insert('end', 'も')
  insert('end', '両方', style_tag_color1, style_tag_color2)
  insert('end', 'とも変えることができます。
')
  insert('end', '
3. 網かけ', style_tag_big)
  insert('end', '  このように描画の際に')
  insert('end', '背景も', style_tag_bgstipple)
  insert('end', '文字も', style_tag_fgstipple)
  insert('end', '単なる塗りつぶし
でなく、網かけを使うことができます。
')
  insert('end', '
4. 下線', style_tag_big)
  insert('end', '  このように')
  insert('end', '文字に下線を引く', style_tag_underline)
  insert('end', 'ことができます。
')
  insert('end', '
5. 打ち消し線', style_tag_big)
  insert('end', '  このように')
  insert('end', '文字に重ねて線を引く', style_tag_overstrike)
  insert('end', 'ことができます。
')
  insert('end', '
6. 3D 効果', style_tag_big)
  insert('end', '  背景に枠をつけて、文字を')
  insert('end', '飛び出す', style_tag_raised)
  insert('end', 'ようにしたり')
  insert('end', '沈む', style_tag_sunken)
  insert('end', '
ようにできます。
')
  insert('end', '
7. 行揃え', style_tag_big)
  insert('end', ' このように行を
')
  insert('end', '左に揃えたり
')
  insert('end', '右に揃えたり
', style_tag_right)
  insert('end', '真中に揃えたりできます。
', style_tag_center)
  insert('end', '
8. 肩付き文字と添字', style_tag_big)
  insert('end', '  10')
  insert('end', 'n', style_tag_super)
  insert('end', ' のように肩付き文字の効果や、')
  insert('end', '
X')
  insert('end', 'i', style_tag_sub)
  insert('end', 'のように添字の効果を出すことができます。
')
  insert('end', '
9. マージン', style_tag_big)
  insert('end', 'テキストの左側に余分な空白を置くことができます:
')
  insert('end', 'この段落はマージンの使用例です。スクリーン',
         style_tag_margins)
  insert('end', '上で折り返されて表示されている1行のテキストです。',
         style_tag_margins)
  insert('end', '左側には2種類のマージンを持ちます。', style_tag_margins)
  insert('end', '1行目に対するものと、', style_tag_margins)
  insert('end', '2行目以降の連続したマージン', style_tag_margins)
  insert('end', 'です。また右側にもマージンがあります。', style_tag_margins)
  insert('end', '行の折り返し位置を決めるために使用することができます。
', style_tag_margins)
  insert('end', '
10. スペーシング', style_tag_big)
  insert('end', '3つのパラメータで行のスペーシングを')
  insert('end', '制御す
ることができます。Spacing1で、行の')
  insert('end', '上にどのくらいの空間を置くか、
spacing3')
  insert('end', 'で行の下にどのくらいの空間を置くか、')
  insert('end', '行が折り返されているなら
ば、spacing2で、')
  insert('end', 'テキスト行を生成している行の間にどのくらい')
  insert('end', 'の空間を置
くかを示します。
')
  insert('end', 'これらのインデントされた段落はどのように',
         style_tag_spacing)
  insert('end', 'スペーシングがが行われるのかを示します。',
         style_tag_spacing)
  insert('end', '各段落は実際はテキストwidget', style_tag_spacing)
  insert('end', 'の1行で、widgetによって折り畳まれています。
', style_tag_spacing)
  insert('end', 'Spacing1はこのテキストでは10pointに', style_tag_spacing)
  insert('end', '設定されています。', style_tag_spacing)
  insert('end', 'これにより、段落の間に大きな間隔が', style_tag_spacing)
  insert('end', '存在しています。', style_tag_spacing)
  insert('end', 'Spacing2は2pointに設定されています。', style_tag_spacing)
  insert('end', 'これで段落の中にほんの少し間隔が存在しています。',
         style_tag_spacing)
  insert('end', 'Spacing3はこの例では使用されていません。
', style_tag_spacing)
  insert('end', '間隔がどこにあるかを見たければ、これらの段落の',
         style_tag_spacing)
  insert('end', 'なかでテキストを選択してください。選択の', style_tag_spacing)
  insert('end', '反転した部分には余分にとられた間隔が', style_tag_spacing)
  insert('end', '含まれています。
', style_tag_spacing)

}

txt.width 70
txt.height 32
