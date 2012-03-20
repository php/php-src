# -*- coding: utf-8 -*-
#
# ruler widget demo (called by 'widget')
#

# rulerMkTab --
# This method creates a new triangular polygon in a canvas to
# represent a tab stop.
#
# Arguments:
# c -           The canvas window.
# x, y -        Coordinates at which to create the tab stop.

def rulerMkTab(c,x,y)
  v = $demo_rulerInfo
  TkcPolygon.new(c, x, y, x+v.size, y+v.size, x-v.size, y+v.size)
end

# toplevel widget が存在すれば削除する
if defined?($ruler_demo) && $ruler_demo
  $ruler_demo.destroy
  $ruler_demo = nil
end

# demo 用の toplevel widget を生成
$ruler_demo = TkToplevel.new {|w|
  title("Ruler Demonstration")
  iconname("ruler")
  positionWindow(w)
}

base_frame = TkFrame.new($ruler_demo).pack(:fill=>:both, :expand=>true)

# label 生成
TkLabel.new(base_frame, 'font'=>$font, 'wraplength'=>'5i', 'justify'=>'left',
            'text'=>"このキャンバスwidgetはルーラーの模型です。ルーラーの右にあるのはタブストップの井戸で、ここから引っ張ってくることによってタブストップを作ることができます。また、すでにあるタブストップを動かすこともできます。タブストップを上方または下方にかすれて表示されるまでドラッグすると、マウスボタンを離した時にそのタブストップは消えます。"){
  pack('side'=>'top')
}

# frame 生成
$ruler_buttons = TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    #text '了解'
    text '閉じる'
    command proc{
      tmppath = $ruler_demo
      $ruler_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'コード参照'
    command proc{showCode 'ruler'}
  }.pack('side'=>'left', 'expand'=>'yes')
}
$ruler_buttons.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# canvas 設定
$ruler_canvas = TkCanvas.new(base_frame, 'width'=>'14.8c', 'height'=>'2.5c')
$ruler_canvas.pack('side'=>'top', 'fill'=>'x')

# 値設定
unless Struct.const_defined?("RulerInfo")
  $demo_rulerInfo = Struct.new("RulerInfo", :grid, :left, :right, :x, :y,
                               :top, :bottom, :size, :normalStyle,
                               :activeStyle, :deleteStyle).new
end
$demo_rulerInfo.grid = '.25c'
$demo_rulerInfo.left = TkWinfo.fpixels($ruler_canvas, '1c')
$demo_rulerInfo.right = TkWinfo.fpixels($ruler_canvas, '13c')
$demo_rulerInfo.top = TkWinfo.fpixels($ruler_canvas, '1c')
$demo_rulerInfo.bottom = TkWinfo.fpixels($ruler_canvas, '1.5c')
$demo_rulerInfo.size = TkWinfo.fpixels($ruler_canvas, '.2c')
$demo_rulerInfo.normalStyle = {'fill'=>'black'}
if TkWinfo.depth($ruler_canvas) > 1
  $demo_rulerInfo.activeStyle = {'fill'=>'red', 'stipple'=>''}
  $demo_rulerInfo.deleteStyle = {'fill'=>'red',
    'stipple'=>'@'+[$demo_dir, '..',
                     'images', 'gray25.xbm'].join(File::Separator)}
else
  $demo_rulerInfo.activeStyle = {'fill'=>'black', 'stipple'=>''}
  $demo_rulerInfo.deleteStyle = {'fill'=>'black',
    'stipple'=>'@'+[$demo_dir, '..',
                     'images', 'gray25.xbm'].join(File::Separator)}
end

TkcLine.new($ruler_canvas,
            '1c', '0.5c', '1c', '1c', '13c', '1c', '13c', '0.5c', 'width'=>1)
(0..11).each{|i|
  x = i+1
  TkcLine.new($ruler_canvas, "#{x}c", '1c', "#{x}c", '0.6c', 'width'=>1)
  TkcLine.new($ruler_canvas, "#{x}.25c", '1c', "#{x}.25c", '0.8c', 'width'=>1)
  TkcLine.new($ruler_canvas, "#{x}.5c", '1c', "#{x}.5c", '0.7c', 'width'=>1)
  TkcLine.new($ruler_canvas, "#{x}.75c", '1c', "#{x}.75c", '0.8c', 'width'=>1)
  TkcText.new($ruler_canvas, "#{x}.15c", '0.75c', 'text'=>i, 'anchor'=>'sw')
}

$rulerTag_well = TkcTag.new($ruler_canvas)
$ruler_canvas\
.addtag_withtag($rulerTag_well,
                TkcRectangle.new($ruler_canvas,
                                 '13.2c', '1c', '13.8c', '0.5c',
                                 'outline'=>'black',
                                 'fill'=>($ruler_canvas\
                                          .configinfo('background'))[4]) )
$ruler_canvas\
.addtag_withtag($rulerTag_well,
                rulerMkTab($ruler_canvas,
                           TkWinfo.pixels($ruler_canvas, '13.5c'),
                           TkWinfo.pixels($ruler_canvas, '.65c') ) )

$rulerTag_well.bind('1', proc{|x,y| rulerNewTab($ruler_canvas,x,y)}, '%x %y')
$ruler_canvas.itembind('tab', '1',
                       proc{|x,y| rulerSelectTab($ruler_canvas,x,y)}, '%x %y')
$ruler_canvas.bind('B1-Motion',
                   proc{|x,y| rulerMoveTab($ruler_canvas,x,y)}, '%x %y')
$ruler_canvas.bind('Any-ButtonRelease-1', proc{rulerReleaseTab($ruler_canvas)})

# rulerNewTab --
# Does all the work of creating a tab stop, including creating the
# triangle object and adding tags to it to give it tab behavior.
#
# Arguments:
# c -           The canvas window.
# x, y -        The coordinates of the tab stop.

def rulerNewTab(c,x,y)
  v = $demo_rulerInfo
  c.addtag_withtag('active', rulerMkTab(c,x,y))
  c.addtag_withtag('tab', 'active')
  v.x = x
  v.y = y
  rulerMoveTab(c,x,y)
end

# rulerSelectTab --
# This method is invoked when mouse button 1 is pressed over
# a tab.  It remembers information about the tab so that it can
# be dragged interactively.
#
# Arguments:
# c -           The canvas widget.
# x, y -        The coordinates of the mouse (identifies the point by
#               which the tab was picked up for dragging).

def rulerSelectTab(c,x,y)
  v = $demo_rulerInfo
  v.x = c.canvasx(x, v.grid)
  v.y = v.top+2
  c.addtag_withtag('active', 'current')
  c.itemconfigure('active', v.activeStyle)
  c.raise('active')
end

# rulerMoveTab --
# This method is invoked during mouse motion events to drag a tab.
# It adjusts the position of the tab, and changes its appearance if
# it is about to be dragged out of the ruler.
#
# Arguments:
# c -           The canvas widget.
# x, y -        The coordinates of the mouse.

def rulerMoveTab(c,x,y)
  v = $demo_rulerInfo
  return if c.find_withtag('active') == []
  cx = c.canvasx(x,v.grid)
  cy = c.canvasy(y)
  cx = v.left if cx < v.left
  cx = v.right if cx > v.right
  if (cy >= v.top && cy <= v.bottom)
    cy = v.top+2
    c.itemconfigure('active', v.activeStyle)
  else
    cy = cy-v.size-2
    c.itemconfigure('active', v.deleteStyle)
  end
  c.move('active', cx-v.x, cy-v.y)
  v.x = cx
  v.y = cy
end

# rulerReleaseTab --
# This method is invoked during button release events that end
# a tab drag operation.  It deselects the tab and deletes the tab if
# it was dragged out of the ruler.
#
# Arguments:
# c -           The canvas widget.
# x, y -        The coordinates of the mouse.

def rulerReleaseTab(c)
  v = $demo_rulerInfo
  return if c.find_withtag('active') == []
  if v.y != v.top+2
    c.delete('active')
  else
    c.itemconfigure('active', v.normalStyle)
    c.dtag('active')
  end
end

