# -*- coding: utf-8 -*-
#
# arrowhead widget demo (called by 'widget')
#

# arrowSetup --
# This method regenerates all the text and graphics in the canvas
# window.  It's called when the canvas is initially created, and also
# whenever any of the parameters of the arrow head are changed
# interactively.
#
# Arguments:
# c -           Name of the canvas widget.

def arrowSetup(c)
  v = $demo_arrowInfo

  # Remember the current box, if there is one.
  tags = c.gettags('current')
  if tags != []
    cur = tags.find{|t| t.kind_of?(String) && t =~ /^box[1-3]$/ }
  else
    cur = nil
  end

  # Create the arrow and outline.
  c.delete('all')
  TkcLine.new(c, v.x1, v.y, v.x2, v.y,
              { 'width'=>10 * v.width,
                'arrowshape'=>[10*v.a, 10*v.b, 10*v.c],
                'arrow'=>'last'
              }.update(v.bigLineStyle) )
  xtip = v.x2 - 10*v.b
  deltaY = 10*v.c + 5*v.width
  TkcLine.new(c, v.x2, v.y, xtip, v.y + deltaY,
              v.x2 - 10*v.a, v.y, xtip, v.y - deltaY, v.x2, v.y,
              'width'=>2, 'capstyle'=>'round', 'joinstyle'=>'round')

  # Create the boxes for reshaping the line and arrowhead.
  TkcRectangle.new(c, v.x2-10*v.a-5, v.y-5, v.x2-10*v.a+5, v.y+5,
                   {'tags'=>['box1', $arrowTag_box]}.update(v.boxStyle) )
  TkcRectangle.new(c, xtip-5, v.y-deltaY-5, xtip+5, v.y-deltaY+5,
                   {'tags'=>['box2', $arrowTag_box]}.update(v.boxStyle) )
  TkcRectangle.new(c, v.x1-5, v.y-5*v.width-5, v.x1+5, v.y-5*v.width+5,
                   {'tags'=>['box3', $arrowTag_box]}.update(v.boxStyle) )
  c.itemconfigure cur, v.activeStyle if cur

  # Create three arrows in actual size with the same parameters
  TkcLine.new(c, v.x2+50, 0, v.x2+50, 1000, 'width'=>2)
  tmp = v.x2+100
  TkcLine.new(c, tmp, v.y-125, tmp, v.y-75, 'width'=>v.width,
              'arrow'=>'both', 'arrowshape'=>[v.a, v.b, v.c])
  TkcLine.new(c, tmp-25, v.y, tmp+25, v.y, 'width'=>v.width,
              'arrow'=>'both', 'arrowshape'=>[v.a, v.b, v.c])
  TkcLine.new(c, tmp-25, v.y+75, tmp+25, v.y+125, 'width'=>v.width,
              'arrow'=>'both', 'arrowshape'=>[v.a, v.b, v.c])

  # Create a bunch of other arrows and text items showing the
  # current dimensions.
  tmp = v.x2+10
  TkcLine.new(c, tmp, v.y-5*v.width, tmp, v.y-deltaY,
              'arrow'=>'both', 'arrowshape'=>v.smallTips)
  TkcText.new(c, v.x2+15, v.y-deltaY+5*v.c, 'text'=>v.c, 'anchor'=>'w')
  tmp = v.x1-10
  TkcLine.new(c, tmp, v.y-5*v.width, tmp, v.y+5*v.width,
              'arrow'=>'both', 'arrowshape'=>v.smallTips)
  TkcText.new(c, v.x1-15, v.y, 'text'=>v.width, 'anchor'=>'e')
  tmp = v.y+5*v.width+10*v.c+10
  TkcLine.new(c, v.x2-10*v.a, tmp, v.x2, tmp,
              'arrow'=>'both', 'arrowshape'=>v.smallTips)
  TkcText.new(c, v.x2-5*v.a, tmp+5, 'text'=>v.a, 'anchor'=>'n')
  tmp = tmp+25
  TkcLine.new(c, v.x2-10*v.b, tmp, v.x2, tmp,
              'arrow'=>'both', 'arrowshape'=>v.smallTips)
  TkcText.new(c, v.x2-5*v.b, tmp+5, 'text'=>v.b, 'anchor'=>'n')

  if $tk_version =~ /^4.*/
    TkcText.new(c, v.x1, 310, 'text'=>"'width'=>#{v.width}", 'anchor'=>'w',
                'font'=>'-*-Helvetica-Medium-R-Normal--*-180-*-*-*-*-*-*')
    TkcText.new(c, v.x1, 330,
                'text'=>"'arrowshape'=>[#{v.a}, #{v.b}, #{v.c}]",'anchor'=>'w',
                'font'=>'-*-Helvetica-Medium-R-Normal--*-180-*-*-*-*-*-*')
  else
    TkcText.new(c, v.x1, 310, 'text'=>"'width'=>#{v.width}", 'anchor'=>'w',
                'font'=>'Helvetica 18')
    TkcText.new(c, v.x1, 330,
                'text'=>"'arrowshape'=>[#{v.a}, #{v.b}, #{v.c}]",
                'anchor'=>'w', 'font'=>'Helvetica 18')
  end

  v.count += 1
end

# toplevel widget が存在すれば削除する
if defined?($arrow_demo) && $arrow_demo
  $arrow_demo.destroy
  $arrow_demo = nil
end

# demo 用の toplevel widget を生成
$arrow_demo = TkToplevel.new {|w|
  title("Arrowhead Editor Demonstration")
  iconname("arrow")
  positionWindow(w)
}

base_frame = TkFrame.new($arrow_demo).pack(:fill=>:both, :expand=>true)

# label 生成
TkLabel.new(base_frame, 'font'=>$font, 'wraplength'=>'5i', 'justify'=>'left',
            'text'=>"この widget で、キャンバスで使われるラインについて様々な幅や矢印の頭の形を試してみることができます。線の幅や矢印の形を変えるには、拡大された矢印についている 3つの四角をドラッグしてください。右側の矢印は普通の大きさでのサンプルを示しています。下のテキストはラインアイテムに対する設定オプションです。"){
  pack('side'=>'top')
}

# frame 生成
$arrow_buttons = TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    #text '了解'
    text '閉じる'
    command proc{
      tmppath = $arrow_demo
      $arrow_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'コード参照'
    command proc{showCode 'arrow'}
  }.pack('side'=>'left', 'expand'=>'yes')
}
$arrow_buttons.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# canvas 設定
$arrow_canvas = TkCanvas.new(base_frame, 'width'=>500, 'height'=>350,
                             'relief'=>'sunken', 'borderwidth'=>2)
$arrow_canvas.pack('expand'=>'yes', 'fill'=>'both')

# 値設定
unless Struct.const_defined?("ArrowInfo")
  $demo_arrowInfo = Struct.new("ArrowInfo", :a, :b, :c, :width, :motionProc,
                               :x1, :x2, :y, :smallTips, :count,
                               :bigLineStyle, :boxStyle, :activeStyle).new
end
$demo_arrowInfo.a = 8
$demo_arrowInfo.b = 10
$demo_arrowInfo.c = 3
$demo_arrowInfo.width = 2
$demo_arrowInfo.motionProc = proc{}
$demo_arrowInfo.x1 = 40
$demo_arrowInfo.x2 = 350
$demo_arrowInfo.y = 150
$demo_arrowInfo.smallTips = [5, 5, 2]
$demo_arrowInfo.count = 0
if TkWinfo.depth($arrow_canvas) > 1
  $demo_arrowInfo.bigLineStyle = {'fill'=>'SkyBlue1'}
  $demo_arrowInfo.boxStyle = {'fill'=>'', 'outline'=>'black', 'width'=>1}
  $demo_arrowInfo.activeStyle = {'fill'=>'red', 'outline'=>'black', 'width'=>1}
else
  $demo_arrowInfo.bigLineStyle = {'fill'=>'black',
    'stipple'=>'@'+[$demo_dir,'..','images','grey.25'].join(File::Separator)}
  $demo_arrowInfo.boxStyle = {'fill'=>'', 'outline'=>'black', 'width'=>1}
  $demo_arrowInfo.activeStyle = {'fill'=>'black','outline'=>'black','width'=>1}
end
$arrowTag_box = TkcTag.new($arrow_canvas)
arrowSetup $arrow_canvas
$arrowTag_box.bind('Enter', proc{$arrow_canvas.itemconfigure('current', $demo_arrowInfo.activeStyle)})
$arrowTag_box.bind('Leave', proc{$arrow_canvas.itemconfigure('current', $demo_arrowInfo.boxStyle)})
$arrowTag_box.bind('B1-Enter', proc{})
$arrowTag_box.bind('B1-Leave', proc{})
$arrow_canvas.itembind('box1', '1',
                       proc{$demo_arrowInfo.motionProc \
                         = proc{|x,y| arrowMove1 $arrow_canvas, x, y}})
$arrow_canvas.itembind('box2', '1',
                       proc{$demo_arrowInfo.motionProc \
                         = proc{|x,y| arrowMove2 $arrow_canvas, x, y}})
$arrow_canvas.itembind('box3', '1',
                       proc{$demo_arrowInfo.motionProc \
                         = proc{|x,y| arrowMove3 $arrow_canvas, x, y}})
$arrowTag_box.bind('B1-Motion',
                  proc{|x,y| $demo_arrowInfo.motionProc.call(x,y)}, "%x %y")
$arrow_canvas.bind('Any-ButtonRelease-1', proc{arrowSetup $arrow_canvas})

# arrowMove1 --
# This method is called for each mouse motion event on box1 (the
# one at the vertex of the arrow).  It updates the controlling parameters
# for the line and arrowhead.
#
# Arguments:
# c -           The name of the canvas window.
# x, y -        The coordinates of the mouse.

def arrowMove1(c,x,y)
  v = $demo_arrowInfo
  newA = (v.x2+5-c.canvasx(x).round)/10
  newA = 0 if newA < 0
  newA = 25 if newA > 25
  if newA != v.a
    c.move('box1', 10*(v.a-newA), 0)
    v.a = newA
  end
end

# arrowMove2 --
# This method is called for each mouse motion event on box2 (the
# one at the trailing tip of the arrowhead).  It updates the controlling
# parameters for the line and arrowhead.
#
# Arguments:
# c -           The name of the canvas window.
# x, y -        The coordinates of the mouse.

def arrowMove2(c,x,y)
  v = $demo_arrowInfo
  newB = (v.x2+5-c.canvasx(x).round)/10
  newB = 0 if newB < 0
  newB = 25 if newB > 25
  newC = (v.y+5-c.canvasy(y).round-5*v.width)/10
  newC = 0 if newC < 0
  newC = 20 if newC > 20
  if newB != v.b || newC != v.c
    c.move('box2', 10*(v.b-newB), 10*(v.c-newC))
    v.b = newB
    v.c = newC
  end
end

# arrowMove3 --
# This method is called for each mouse motion event on box3 (the
# one that controls the thickness of the line).  It updates the
# controlling parameters for the line and arrowhead.
#
# Arguments:
# c -           The name of the canvas window.
# x, y -        The coordinates of the mouse.

def arrowMove3(c,x,y)
  v = $demo_arrowInfo
  newWidth = (v.y+2-c.canvasy(y).round)/5
  newWidth = 0 if newWidth < 0
  newWidth = 20 if newWidth > 20
  if newWidth != v.width
    c.move('box3', 0, 5*(v.width-newWidth))
    v.width = newWidth
  end
end

