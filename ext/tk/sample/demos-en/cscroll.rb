# cscroll.rb
#
# This demonstration script creates a simple canvas that can be
# scrolled in two dimensions.
#
# simple scrollable canvas widget demo (called by 'widget')
#

# toplevel widget
if defined?($cscroll_demo) && $cscroll_demo
  $cscroll_demo.destroy
  $cscroll_demo = nil
end

# demo toplevel widget
$cscroll_demo = TkToplevel.new {|w|
  title("Scrollable Canvas Demonstration")
  iconname("cscroll")
  positionWindow(w)
}

base_frame = TkFrame.new($cscroll_demo).pack(:fill=>:both, :expand=>true)

# label
TkLabel.new(base_frame, 'font'=>$font, 'wraplength'=>'4i',
            'justify'=>'left', 'text'=>"This window displays a canvas widget that can be scrolled either using the scrollbars or by dragging with button 2 in the canvas.  If you click button 1 on one of the rectangles, its indices will be printed on stdout."){
  pack('side'=>'top')
}

# frame
$cscroll_buttons = TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    text 'Dismiss'
    command proc{
      tmppath = $cscroll_demo
      $cscroll_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'Show Code'
    command proc{showCode 'cscroll'}
  }.pack('side'=>'left', 'expand'=>'yes')
}
$cscroll_buttons.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# frame
unless $tk_version =~ /^4\.[01]/
  $cscroll_grid = TkFrame.new(base_frame) {
    pack('expand'=>'yes', 'fill'=>'both', 'padx'=>1, 'pady'=>1)
  }
  TkGrid.rowconfigure($cscroll_grid, 0, 'weight'=>1, 'minsize'=>0)
  TkGrid.columnconfigure($cscroll_grid, 0, 'weight'=>1, 'minsize'=>0)
end

# canvas
$cscroll_canvas = TkCanvas.new(base_frame,
                               'relief'=>'sunken', 'borderwidth'=>2,
                               'scrollregion'=>['-11c', '-11c', '50c', '20c']
                               ) {|c|
  if $tk_version =~ /^4\.[01]/
    pack('expand'=>'yes', 'fill'=>'both')
  else
    grid('in'=>$cscroll_grid, 'padx'=>1, 'pady'=>1, 'row'=>0, 'column'=>0,
         'rowspan'=>1, 'columnspan'=>1, 'sticky'=>'news')
  end

  TkScrollbar.new(base_frame, 'command'=>proc{|*args| c.yview(*args)}) {|vs|
    c.yscrollcommand(proc{|first,last| vs.set first,last})
    if $tk_version =~ /^4\.[01]/
      pack('side'=>'right', 'fill'=>'y')
    else
      grid('in'=>$cscroll_grid, 'padx'=>1, 'pady'=>1, 'row'=>0, 'column'=>1,
           'rowspan'=>1, 'columnspan'=>1, 'sticky'=>'news')
    end
  }

  TkScrollbar.new(base_frame, 'orient'=>'horiz',
                  'command'=>proc{|*args| c.xview(*args)}) {|hs|
    c.xscrollcommand(proc{|first,last| hs.set first,last})
    if $tk_version =~ /^4\.[01]/
      pack('side'=>'bottom', 'fill'=>'x')
    else
      grid('in'=>$cscroll_grid, 'padx'=>1, 'pady'=>1, 'row'=>1, 'column'=>0,
           'rowspan'=>1, 'columnspan'=>1, 'sticky'=>'news')
    end
  }
}

bg = $cscroll_canvas.configinfo('bg')[4]
(0..19).each{|i|
  x = -10+3*i
  y = -10
  (0..9).each{|j|
    TkcRectangle.new($cscroll_canvas, "#{x}c", "#{y}c", "#{x+2}c", "#{y+2}c",
                     'outline'=>'black', 'fill'=>bg, 'tags'=>'rect')
    TkcText.new($cscroll_canvas, "#{x+1}c", "#{y+1}c",
                'text'=>"#{i},#{j}", 'anchor'=>'center', 'tags'=>'text')
    y += 3
  }
}

$cscroll_canvas.itembind('all', 'Any-Enter', proc{scrollEnter $cscroll_canvas})
$cscroll_canvas.itembind('all', 'Any-Leave', proc{scrollLeave $cscroll_canvas})
$cscroll_canvas.itembind('all', '1', proc{scrollButton $cscroll_canvas})
$cscroll_canvas.itembind('all', 'Any-Enter', proc{scrollEnter $cscroll_canvas})
$cscroll_canvas.bind('2', proc{|x,y| $cscroll_canvas.scan_mark(x,y)}, '%x %y')
$cscroll_canvas.bind('B2-Motion',
                     proc{|x,y| $cscroll_canvas.scan_dragto(x,y)}, '%x %y')

def scrollEnter(c)
  id = c.find_withtag('current')[0].id
  id -= 1 if c.gettags('current').include?('text')
  $oldFill = c.itemconfiginfo(id, 'fill')[4]
  if TkWinfo.depth(c) > 1
    c.itemconfigure(id, 'fill'=>'SeaGreen1')
  else
    c.itemconfigure(id, 'fill'=>'black')
    c.itemconfigure(id+1, 'fill'=>'white')
  end
end

def scrollLeave(c)
  id = c.find_withtag('current')[0].id
  id -= 1 if c.gettags('current').include?('text')
  c.itemconfigure(id, 'fill'=>$oldFill)
  c.itemconfigure(id+1, 'fill'=>'black')
end

def scrollButton(c)
  id = c.find_withtag('current')[0].id
  id += 1 unless c.gettags('current').include?('text')
  print "You buttoned at #{c.itemconfiginfo(id,'text')[4]}\n"
end

