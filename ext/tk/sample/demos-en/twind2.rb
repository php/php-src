#
# text (embedded windows) widget demo 2 (called by 'widget')
#

# delete toplevel widget
if defined?($twind2_demo) && $twind2_demo
  $twind2_demo.destroy
  $twind2_demo = nil
end

# demo toplevel widget
$twind2_demo = TkToplevel.new {|w|
  title("Text Demonstration - Embedded Windows 2")
  iconname("Embedded Windows")
  positionWindow(w)
}

base_frame = TkFrame.new($twind2_demo).pack(:fill=>:both, :expand=>true)

# frame
$twind2_buttons = TkFrame.new(base_frame) {|frame|
  TkGrid(TkFrame.new(frame, :height=>2, :relief=>:sunken, :bd=>2),
         :columnspan=>4, :row=>0, :sticky=>'ew', :pady=>2)
  TkGrid('x',
         TkButton.new(frame, :text=>'See Code',
                      :image=>$image['view'], :compound=>:left,
                      :command=>proc{showCode 'twind2'}),
         TkButton.new(frame, :text=>'Dismiss',
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

# frame
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

# text tags
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

# insert text
$twind2_text.insert('end',
                  'A text widget can contain many different kinds of items, ')
$twind2_text.insert('end',
                  "both active and passive.  It can lay these out in various ")
$twind2_text.insert('end',
                   "ways, with wrapping, tabs, centering, etc.  In addition, ")
$twind2_text.insert('end',
                    "when the contents are too big for the window, smooth ")
$twind2_text.insert('end', "scrolling in all directions is provided.\n\n")

$twind2_text.insert('end', "A text widget can contain other widgets embedded ")
$twind2_text.insert('end', "it.  These are called \"embedded windows\", ")
$twind2_text.insert('end', "and they can consist of arbitrary widgets.  ")
$twind2_text.insert('end', "For example, here are two embedded button ")
$twind2_text.insert('end', "widgets.  You can click on the first button to ")
TkTextWindow.new($twind2_text, 'end',
                 'window'=>TkButton.new($twind2_text) {
                   text 'ON'
                   command proc{textWindOn2 $twind2_text,$twind2_buttons}
                   cursor 'top_left_arrow'
                 })
$twind2_text.insert('end', " horizontal scrolling, which also turns off ")
$twind2_text.insert('end', "word wrapping.  Or, you can click on the second ")
$twind2_text.insert('end', "button to\n")
TkTextWindow.new($twind2_text, 'end',
                 'window'=>TkButton.new($twind2_text) {
                   text 'OFF'
                   command proc{textWindOff2 $twind2_text}
                   cursor 'top_left_arrow'
                 })
$twind2_text.insert('end',
                   " horizontal scrolling and turn back on word wrapping.\n\n")

$twind2_text.insert('end', "Or, here is another example.  If you ")
TkTextWindow.new($twind2_text, 'end',
                 'window'=>TkButton.new($twind2_text) {
                   text 'Click Here'
                   command proc{textWindPlot2 $twind2_text}
                   cursor 'top_left_arrow'
                 })
$twind2_text.insert('end',
                    " a canvas displaying an x-y plot will appear right here.")
$mark2_plot = TkTextMark.new($twind2_text, 'insert')
$mark2_plot.gravity='left'
$twind2_text.insert('end',
                    "  You can drag the data points around with the mouse, ")
$twind2_text.insert('end', "or you can click here to ")
TkTextWindow.new($twind2_text, 'end',
                 'window'=>TkButton.new($twind2_text) {
                   text 'Delete'
                   command proc{textWindDel2 $twind2_text}
                   cursor 'top_left_arrow'
                 })
$twind2_text.insert('end', " the plot again.\n\n")

$twind2_text.insert('end',
                    "You may also find it useful to put embedded windows in ")
$twind2_text.insert('end',
                    "a text without any actual text.  In this case the ")
$twind2_text.insert('end', "text widget acts like a geometry manager.  For ")
$twind2_text.insert('end',
                    "example, here is a collection of buttons laid out ")
$twind2_text.insert('end',
                    "neatly into rows by the text widget.  These buttons ")
$twind2_text.insert('end',
                    "can be used to change the background color of the ")
$twind2_text.insert('end', "text widget (\"Default\" restores the color to ")
$twind2_text.insert('end',
                    "its default).  If you click on the button labeled ")
$twind2_text.insert('end', "\"Short\", it changes to a longer string so that ")
$twind2_text.insert('end', "you can see how the text widget automatically ")
$twind2_text.insert('end', "changes the layout.  Click on the button again ")
$twind2_text.insert('end', "to restore the short string.\n")

btn_default = TkButton.new($twind2_text) {|b|
  text 'Default'
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

$twind2_text.insert('end', "\nYou can also change the usual border width and ")
$twind2_text.insert('end', "highlightthickness and padding.\n")

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

$twind2_text.insert('end',
                    "\n\nFinally, images fit comfortably in text widgets too:")

TkTextImage.new($twind2_text, 'end',
                'image'=>TkBitmapImage.new(:file=>[
                                             $demo_dir, '..',
                                             'images', 'face.xbm'
                                           ].join(File::Separator)))

# methods
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
