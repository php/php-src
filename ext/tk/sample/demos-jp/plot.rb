# -*- coding: utf-8 -*-
#
# 2-D plot widget demo (called by 'widget')
#

# toplevel widget が存在すれば削除する
if defined?($plot_demo) && $plot_demo
  $plot_demo.destroy
  $plot_demo = nil
end

# demo 用の toplevel widget を生成
$plot_demo = TkToplevel.new {|w|
  title("Plot Demonstration")
  iconname("Plot")
  positionWindow(w)
}

base_frame = TkFrame.new($plot_demo).pack(:fill=>:both, :expand=>true)

# label 生成
TkLabel.new(base_frame, 'font'=>$font, 'wraplength'=>'4i', 'justify'=>'left',
            'text'=>"このウィンドウは簡単な2次元のプロットを含んだキャンバス widgetです。表示された点をマウスボタン1でドラッグしてデータをいじることができます。"){
  pack('side'=>'top')
}

# frame 生成
$plot_buttons = TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    #text '了解'
    text '閉じる'
    command proc{
      tmppath = $plot_demo
      $plot_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'コード参照'
    command proc{showCode 'plot'}
  }.pack('side'=>'left', 'expand'=>'yes')
}
$plot_buttons.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# font 設定
  if $tk_version =~ /^4.*/
    plotFont = '-*-Helvetica-Medium-R-Normal--*-180-*-*-*-*-*-*'
  else
    font = 'Helvetica 18'
  end

# canvas 設定
$plot_canvas = TkCanvas.new(base_frame,'relief'=>'raised','width'=>450,'height'=>300)
$plot_canvas.pack('side'=>'top', 'fill'=>'x')

# plot 生成
TkcLine.new($plot_canvas, 100, 250, 400, 250, 'width'=>2)
TkcLine.new($plot_canvas, 100, 250, 100,  50, 'width'=>2)
TkcText.new($plot_canvas, 225, 20,
            'text'=>"簡単なプロット", 'font'=>plotFont, 'fill'=>'brown')

(0..10).each {|i|
  x = 100 + (i * 30)
  TkcLine.new($plot_canvas, x, 250, x, 245, 'width'=>2)
  TkcText.new($plot_canvas, x, 254,
              'text'=>10*i, 'font'=>plotFont, 'anchor'=>'n')
}
(0..5).each {|i|
  y = 250 - (i * 40)
  TkcLine.new($plot_canvas, 100, y, 105, y, 'width'=>2)
  TkcText.new($plot_canvas, 96, y,
              'text'=>"#{i*50}.0", 'font'=>plotFont, 'anchor'=>'e')
}

for xx, yy in [[12,56],[20,94],[33,98],[32,120],[61,180],[75,160],[98,223]]
  x = 100 + (3*xx)
  y = 250 - (4*yy)/5
  item = TkcOval.new($plot_canvas, x-6, y-6, x+6, y+6,
                     'width'=>1, 'outline'=>'black', 'fill'=>'SkyBlue2')
  item.addtag 'point'
end

$plot_canvas.itembind('point', 'Any-Enter',
                      proc{$plot_canvas.itemconfigure 'current','fill','red'})
$plot_canvas.itembind('point', 'Any-Leave',
                      proc{$plot_canvas.itemconfigure 'current','fill','SkyBlue2'})
$plot_canvas.itembind('point', '1',
                      proc{|x,y| plotDown $plot_canvas,x,y}, "%x %y")
$plot_canvas.itembind('point', 'ButtonRelease-1',
                      proc{$plot_canvas.dtag 'selected'})
$plot_canvas.bind('B1-Motion',
                  proc{|x,y| plotMove $plot_canvas,x,y}, "%x %y")

$plot = {'lastX'=>0, 'lastY'=>0}

# plotDown --
# This method is invoked when the mouse is pressed over one of the
# data points.  It sets up state to allow the point to be dragged.
#
# Arguments:
# w -           The canvas window.
# x, y -        The coordinates of the mouse press.

def plotDown (w, x, y)
  w.dtag 'selected'
  w.addtag_withtag 'selected', 'current'
  w.raise 'current'
  $plot['lastX'] = x
  $plot['lastY'] = y
end

# plotMove --
# This method is invoked during mouse motion events.  It drags the
# current item.
#
# Arguments:
# w -           The canvas window.
# x, y -        The coordinates of the mouse.

def plotMove (w, x, y)
  w.move 'selected', x - $plot['lastX'], y - $plot['lastY']
  $plot['lastX'] = x
  $plot['lastY'] = y
end

