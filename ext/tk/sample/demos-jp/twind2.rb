# -*- coding: utf-8 -*-
#
# text (embedded windows) widget demo 2 (called by 'widget')
#

# toplevel widget が存在すれば削除する
if defined?($twind2_demo) && $twind2_demo
  $twind2_demo.destroy
  $twind2_demo = nil
end

# demo 用の toplevel widget を生成
$twind2_demo = TkToplevel.new {|w|
  title("Text Demonstration - Embedded Windows 2")
  iconname("Embedded Windows")
  positionWindow(w)
}

base_frame = TkFrame.new($twind2_demo).pack(:fill=>:both, :expand=>true)

# frame 生成
$twind2_buttons = TkFrame.new(base_frame) {|frame|
  TkGrid(TkFrame.new(frame, :height=>2, :relief=>:sunken, :bd=>2),
         :columnspan=>4, :row=>0, :sticky=>'ew', :pady=>2)
  TkGrid('x',
         TkButton.new(frame, :text=>'コード参照',
                      :image=>$image['view'], :compound=>:left,
                      :command=>proc{showCode 'twind2'}),
         TkButton.new(frame, :text=>'閉じる',
                      :image=>$image['delete'], :compound=>:left,
                      :command=>proc{
                        tmppath = $twind2_demo
                        $twind2_demo = nil
                        $showVarsWin[tmppath.path] = nil
                        tmppath.destroy
                      }),
         :padx=>4, :pady=>4)
  frame.grid_columnconfigure(0, :weight=>1)
}
$twind2_buttons.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# frame 生成
$twind2_text = nil
TkFrame.new(base_frame, 'highlightthickness'=>2, 'borderwidth'=>2,
            'relief'=>'sunken') {|f|
  $twind2_text = TkText.new(f, 'setgrid'=>true, 'font'=>$font,
                            # 'width'=>'70', 'height'=>35, 'wrap'=>'word',
                            'width'=>'70', 'height'=>35, 'wrap'=>'char',
                            'highlightthickness'=>0, 'borderwidth'=>0 ){|t|
    TkScrollbar.new(f) {|s|
      command proc{|*args| t.yview(*args)}
      t.yscrollcommand proc{|first,last| s.set first,last}
    }.pack('side'=>'right', 'fill'=>'y')
  }.pack('expand'=>'yes', 'fill'=>'both')
}.pack('expand'=>'yes', 'fill'=>'both')

# タグ生成
$tag2_center = TkTextTag.new($twind2_text,
                            'justify' =>'center',
                            'spacing1'=>'5m',
                            'spacing3'=>'5m'  )
$tag2_buttons = TkTextTag.new($twind2_text,
                             'lmargin1'=>'1c',
                             'lmargin2'=>'1c',
                             'rmargin' =>'1c',
                             'spacing1'=>'3m',
                             'spacing2'=>0,
                             'spacing3'=>0 )

# テキストの生成
$twind2_text.insert('end', 'テキストウィジェットには色々な種類のアイテム')
$twind2_text.insert('end', '（動的なものや静的なものの両方があります）を')
$twind2_text.insert('end', '数多く含めることができます。そうしたアイテムは')
$twind2_text.insert('end', '行替え、タブ、中央揃えなどの様々な方法で')
$twind2_text.insert('end', '配置することができます。')
$twind2_text.insert('end', '加えて、テキストウィジェットの内容物が')
$twind2_text.insert('end', 'ウィンドウサイズに比べて大きすぎる場合でも')
$twind2_text.insert('end', 'すべての方向にスムーズにスクロールさせて')
$twind2_text.insert('end', '確認することが可能です。')
$twind2_text.insert('end', "\n\n")
$twind2_text.insert('end', 'テキストウィジェット上には他のウィジェットを')
$twind2_text.insert('end', '含めることもできます。こうしたものは')
$twind2_text.insert('end', '「埋め込みウィンドウ」と呼ばれ、その中に')
$twind2_text.insert('end', 'いかなるウィジェットでも詰め込むことができます。')
$twind2_text.insert('end', '例えば，ここには２つの')
$twind2_text.insert('end', 'ボタンウィジェットが埋め込まれています。')
$twind2_text.insert('end', '最初のボタンをクリックすると、')
$twind2_text.insert('end', '水平方向のスクロールを ')
TkTextWindow.new($twind2_text, 'end',
                 'window'=>TkButton.new($twind2_text) {
                   #text 'ON'
                   text 'オン'
                   command proc{textWindOn2 $twind2_text,$twind2_buttons}
                   cursor 'top_left_arrow'
                 })
$twind2_text.insert('end', "にします。また、２つめのボタンをクリックすると\n")
$twind2_text.insert('end', '水平方向のスクロールを')
TkTextWindow.new($twind2_text, 'end',
                 'window'=>TkButton.new($twind2_text) {
                   #text 'OFF'
                   text 'オフ'
                   command proc{textWindOff2 $twind2_text}
                   cursor 'top_left_arrow'
                 })
$twind2_text.insert('end', "にします。\n\n")

$twind2_text.insert('end', '次はもうひとつの例です。')
TkTextWindow.new($twind2_text, 'end',
                 'window'=>TkButton.new($twind2_text) {
                   text 'ここをクリック'
                   command proc{textWindPlot2 $twind2_text}
                   cursor 'top_left_arrow'
                 })
$twind2_text.insert('end', 'すると、x-yプロットがここに現れます。')
$mark2_plot = TkTextMark.new($twind2_text, 'insert')
$mark2_plot.gravity='left'
$twind2_text.insert('end', 'マウスでドラッグすることで、')
$twind2_text.insert('end', 'プロット上のデータ点を移動することができます。')
TkTextWindow.new($twind2_text, 'end',
                 'window'=>TkButton.new($twind2_text) {
                   text '消去'
                   command proc{textWindDel2 $twind2_text}
                   cursor 'top_left_arrow'
                 })
$twind2_text.insert('end', 'をクリックすると元に戻ります。')
$twind2_text.insert('end', "\n\n")

$twind2_text.insert('end', '表示するテキストなしに埋め込みウィンドウだけを')
$twind2_text.insert('end', 'テキストウィジェットに含めることも有用でしょう。')
$twind2_text.insert('end', 'この場合、テキストウィジェットは')
$twind2_text.insert('end', 'ジオメトリマネージャのように働きます。')
$twind2_text.insert('end', '例えば、ここにはテキストウィジェットに')
$twind2_text.insert('end', 'よってボタンがきれいに整列して配置されています。')
$twind2_text.insert('end', 'これらのボタンをクリックすることで、')
$twind2_text.insert('end', 'このテキストウィジェットの背景色を')
$twind2_text.insert('end', '変えることができます("デフォルト"ボタンで')
$twind2_text.insert('end', '元の色に戻すことができます)。')
$twind2_text.insert('end', '"Short"というボタンをクリックすると文字列の長さが')
$twind2_text.insert('end', '変わり、テキストウィジェットが自動的に')
$twind2_text.insert('end', 'レイアウトを整える様子を見ることができます。')
$twind2_text.insert('end', 'もう一度同じボタンを押すと元に戻ります。')
$twind2_text.insert('end', "\n")

btn_default = TkButton.new($twind2_text) {|b|
  text 'デフォルト'
  command proc{embDefBg2 $twind2_text}
  cursor 'top_left_arrow'
}
TkTextWindow.new($twind2_text, 'end', 'window'=>btn_default, 'padx'=>3)
embToggle = TkVariable.new('Short')
TkTextWindow.new($twind2_text, 'end',
                 'window'=>TkCheckButton.new($twind2_text) {
                   textvariable embToggle
                   indicatoron 0
                   variable embToggle
                   onvalue 'A much longer string'
                   offvalue 'Short'
                   cursor 'top_left_arrow'
                   pady 5
                   padx 2
                 },
                 'padx'=>3,
                 'pady'=>2 )

[ 'AntiqueWhite3', 'Bisque1', 'Bisque2', 'Bisque3', 'Bisque4',
  'SlateBlue3', 'RoyalBlue1', 'SteelBlue2', 'DeepSkyBlue3', 'LightBlue1',
  'DarkSlateGray1', 'Aquamarine2', 'DarkSeaGreen2', 'SeaGreen1',
  'Yellow1', 'IndianRed1', 'IndianRed2', 'Tan1', 'Tan4'
].each{|twind_color|
  TkTextWindow.new($twind2_text, 'end',
                   'window'=>TkButton.new($twind2_text) {
                     text twind_color
                     cursor 'top_left_arrow'
                     command proc{$twind2_text.bg twind_color}
                   },
                   'padx'=>3,
                   'pady'=>2 )
}

$tag2_buttons.add(btn_default, 'end')

$text_normal2 = {}
$text_normal2['border'] = $twind2_text.cget('borderwidth')
$text_normal2['highlight'] = $twind2_text.cget('highlightthickness')
$text_normal2['pad'] = $twind2_text.cget('padx')

$twind2_text.insert('end', "\nborder width や highlightthickness, ")
$twind2_text.insert('end', "padding を通常の値から変更することも可能です。\n")

TkTextWindow.new($twind2_text, 'end',
                 'window'=>TkButton.new($twind2_text, :text=>"Big borders",
                                        :cursor=>'top_left_arrow',
                                        'command'=>proc{
                                          textWinBigB2 $twind2_text
                                        }))

TkTextWindow.new($twind2_text, 'end',
                 'window'=>TkButton.new($twind2_text, :text=>"Small borders",
                                        :cursor=>'top_left_arrow',
                                        'command'=>proc{
                                          textWinSmallB2 $twind2_text
                                        }))

TkTextWindow.new($twind2_text, 'end',
                 'window'=>TkButton.new($twind2_text, :text=>"Big highlight",
                                        :cursor=>'top_left_arrow',
                                        'command'=>proc{
                                          textWinBigH2 $twind2_text
                                        }))

TkTextWindow.new($twind2_text, 'end',
                 'window'=>TkButton.new($twind2_text, :text=>"Small highlight",
                                        :cursor=>'top_left_arrow',
                                        'command'=>proc{
                                          textWinSmallH2 $twind2_text
                                        }))

TkTextWindow.new($twind2_text, 'end',
                 'window'=>TkButton.new($twind2_text, :text=>"Big pad",
                                        :cursor=>'top_left_arrow',
                                        'command'=>proc{
                                          textWinBigP2 $twind2_text
                                        }))

TkTextWindow.new($twind2_text, 'end',
                 'window'=>TkButton.new($twind2_text, :text=>"Small pad",
                                        :cursor=>'top_left_arrow',
                                        'command'=>proc{
                                          textWinSmallP2 $twind2_text
                                        }))

$twind2_text.insert('end', "\n\n更にイメージもテキストウィジェットに")
$twind2_text.insert('end', "うまく配置できます：")

TkTextImage.new($twind2_text, 'end',
                'image'=>TkBitmapImage.new(:file=>[
                                             $demo_dir, '..',
                                             'images', 'face.xbm'
                                           ].join(File::Separator)))

# メソッド定義
def textWinBigB2(w)
  w.borderwidth 15
end
def textWinSmallB2(w)
  w.borderwidth $text_normal2['border']
end
def textWinBigH2(w)
  w.highlightthickness 15
end
def textWinSmallH2(w)
  w.highlightthickness $text_normal2['highlight']
end
def textWinBigP2(w)
  w.configure(:padx=>15, :pady=>15)
end
def textWinSmallP2(w)
  w.configure(:padx=>$text_normal2['pad'], :pady=>$text_normal2['pad'])
end

def textWindOn2 (w,f)
  if defined? $twind2_scroll
    begin
      $twind2_scroll.destroy
    rescue
    end
    $twind2_scroll = nil
  end

  base = TkWinfo.parent( TkWinfo.parent(w) )
  $twind2_scroll = TkScrollbar.new(base) {|s|
    orient 'horizontal'
    command proc{|*args| w.xview(*args)}
    w.xscrollcommand proc{|first,last| s.set first,last}
    w.wrap 'none'
    pack('after'=>f, 'side'=>'bottom', 'fill'=>'x')
  }

  return nil
end

def textWindOff2 (w)
  if defined? $twind2_scroll
    begin
      $twind2_scroll.destroy
    rescue
    end
    $twind2_scroll = nil
  end
  w.xscrollcommand ''
  #w.wrap 'word'
  w.wrap 'char'
end

def textWindPlot2 (t)
  if (defined? $twind2_plot) && (TkWinfo.exist?($twind2_plot))
    return
  end

  $twind2_plot = TkCanvas.new(t) {
    relief 'sunken'
    width  450
    height 300
    cursor 'top_left_arrow'
  }

  #font = '-Adobe-Helvetica-Medium-R-Normal--*-180-*-*-*-*-*-*'
  font = 'Helvetica 18'

  TkcLine.new($twind2_plot, 100, 250, 400, 250, 'width'=>2)
  TkcLine.new($twind2_plot, 100, 250, 100,  50, 'width'=>2)
  TkcText.new($twind2_plot, 225, 20,
              'text'=>"A Simple Plot", 'font'=>font, 'fill'=>'brown')

  (0..10).each {|i|
    x = 100 + (i * 30)
    TkcLine.new($twind2_plot, x, 250, x, 245, 'width'=>2)
    TkcText.new($twind2_plot, x, 254,
                'text'=>10*i, 'font'=>font, 'anchor'=>'n')
  }
  (0..5).each {|i|
    y = 250 - (i * 40)
    TkcLine.new($twind2_plot, 100, y, 105, y, 'width'=>2)
    TkcText.new($twind2_plot, 96, y,
                'text'=>"#{i*50}.0", 'font'=>font, 'anchor'=>'e')
  }

  for xx, yy in [[12,56],[20,94],[33,98],[32,120],[61,180],[75,160],[98,223]]
    x = 100 + (3*xx)
    y = 250 - (4*yy)/5
    item = TkcOval.new($twind2_plot, x-6, y-6, x+6, y+6,
                       'width'=>1, 'outline'=>'black', 'fill'=>'SkyBlue2')
    item.addtag 'point'
  end

  $twind2_plot.itembind('point', 'Any-Enter',
                        proc{$twind2_plot.itemconfigure 'current', 'fill', 'red'})
  $twind2_plot.itembind('point', 'Any-Leave',
                        proc{$twind2_plot.itemconfigure 'current', 'fill', 'SkyBlue2'})
  $twind2_plot.itembind('point', '1',
                        proc{|x,y| embPlotDown2 $twind2_plot,x,y}, "%x %y")
  $twind2_plot.itembind('point', 'ButtonRelease-1',
                        proc{$twind2_plot.dtag 'selected'})
  $twind2_plot.bind('B1-Motion',
                    proc{|x,y| embPlotMove2 $twind2_plot,x,y}, "%x %y")
  while ($twind2_text.get($mark2_plot) =~ /[ \t\n]/)
    $twind2_text.delete $mark2_plot
  end
  $twind2_text.insert $mark2_plot,"\n"
  TkTextWindow.new($twind2_text, $mark2_plot, 'window'=>$twind2_plot)
  $tag2_center.add $mark2_plot
  $twind2_text.insert $mark2_plot,"\n"
end

$embPlot2 = {'lastX'=>0, 'lastY'=>0}

def embPlotDown2 (w, x, y)
  w.dtag 'selected'
  w.addtag_withtag 'selected', 'current'
  w.raise 'current'
  $embPlot2['lastX'] = x
  $embPlot2['lastY'] = y
end

def embPlotMove2 (w, x, y)
  w.move 'selected', x - $embPlot2['lastX'], y - $embPlot2['lastY']
  $embPlot2['lastX'] = x
  $embPlot2['lastY'] = y
end

def textWindDel2 (w)
  if (defined? $twind2_text) && TkWinfo.exist?($twind2_plot)
    $twind2_text.delete $twind2_plot
    $twind2_plot = nil
    while ($twind2_text.get($mark2_plot) =~ /[ \t\n]/)
      $twind2_text.delete $mark2_plot
    end
    $twind2_text.insert $mark2_plot,"  "
  end
end

def embDefBg2 (w)
  w['background'] = w.configinfo('background')[3]
end
