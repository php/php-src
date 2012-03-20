# items.rb
#
# This demonstration script creates a canvas that displays the
# canvas item types.
#
# canvas item types widget demo (called by 'widget')
#

# toplevel widget
if defined?($items_demo) && $items_demo
  $items_demo.destroy
  $items_demo = nil
end

# demo toplevel widget
$items_demo = TkToplevel.new {|w|
  title("Canvas Item Demonstration")
  iconname("Items")
  positionWindow(w)
}

base_frame = TkFrame.new($items_demo).pack(:fill=>:both, :expand=>true)

# label
TkLabel.new(base_frame) {
  font $font
  wraplength '5i'
  justify 'left'
  text "This window contains a canvas widget with examples of the various kinds of items supported by canvases.  The following operations are supported:\n  Button-1 drag:\tmoves item under pointer.\n  Button-2 drag:\trepositions view.\n  Button-3 drag:\tstrokes out area.\n  Ctrl+f:\t\tprints items under area."
}.pack('side'=>'top')

# frame
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    text 'Dismiss'
    command proc{
      tmppath = $items_demo
      $items_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'Show Code'
    command proc{showCode 'items'}
  }.pack('side'=>'left', 'expand'=>'yes')
}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# frame
cvs = nil
TkFrame.new(base_frame) {|cf|
  # canvas
  cvs = TkCanvas.new(cf) {|c|
    focus
    scrollregion '0c 0c 30c 24c'
    width  '15c'
    height '10c'
    relief 'sunken'
    borderwidth 2

    hs = TkScrollbar.new(cf) {|s|
      orient 'horizontal'
      command proc{|*args| c.xview(*args)}
      c.xscrollcommand proc{|first,last| s.set first,last}
    }

    vs = TkScrollbar.new(cf) {|s|
      command proc{|*args| c.yview(*args)}
      c.yscrollcommand proc{|first,last| s.set first,last}
    }

    if $tk_version =~ /^4\.[01]/
      hs.pack('side'=>'bottom', 'fill'=>'x')
      vs.pack('side'=>'right', 'fill'=>'y')
      c.pack('in'=>cf, 'expand'=>'yes', 'fill'=>'both')

    else
      c.grid('in'=>cf, 'row'=>0, 'column'=>0,
             'rowspan'=>1, 'columnspan'=>1, 'sticky'=>'news')
      vs.grid('row'=>0, 'column'=>1,
              'rowspan'=>1, 'columnspan'=>1, 'sticky'=>'news')
      hs.grid('row'=>1, 'column'=>0,
              'rowspan'=>1, 'columnspan'=>1, 'sticky'=>'news')
      TkGrid.rowconfigure(cf, 0, 'weight'=>1, 'minsize'=>0)
      TkGrid.columnconfigure(cf, 0, 'weight'=>1, 'minsize'=>0)

    end

  }
}.pack('side'=>'top', 'fill'=>'both', 'expand'=>'yes')

# Display a 3x3 rectangular grid
TkcRectangle.new(cvs, '0c', '0c', '30c', '24c', 'width'=>2)
TkcLine.new(cvs, '0c', '8c', '30c', '8c', 'width'=>2)
TkcLine.new(cvs, '0c', '16c', '30c', '16c', 'width'=>2)
TkcLine.new(cvs, '10c', '0c', '10c', '24c', 'width'=>2)
TkcLine.new(cvs, '20c', '0c', '20c', '24c', 'width'=>2)

if $tk_version =~ /^4.*/
  font1 = '-Adobe-Helvetica-Medium-R-Normal--*-120-*-*-*-*-*-*'
  font2 = '-Adobe-Helvetica-Bold-R-Normal--*-240-*-*-*-*-*-*'
else
  font1 = 'Helvetica 12'
  font2 = 'Helvetica 24 bold'
end
if TkWinfo.depth($root).to_i > 1
  blue   = 'DeepSkyBlue3'
  red    = 'red'
  bisque = 'bisque3'
  green  = 'SeaGreen3'
else
  blue   = 'black'
  red    = 'black'
  bisque = 'black'
  green  = 'black'
end

# tag
$tag_item = TkcGroup.new(cvs)

# Set up demos within each of the areas of the grid.
TkcText.new(cvs, '5c', '.2c', 'text'=>'Lines', 'anchor'=>'n')
TkcLine.new(cvs, '1c', '1c', '3c', '1c', '1c', '4c', '3c', '4c',
            'width'=>2, 'fill'=>blue, 'capstyle'=>'butt',
            'join'=>'miter', 'tags'=>$tag_item )
TkcLine.new(cvs, '4.67c','1c','4.67c','4c', 'arrow'=>'last', 'tags'=>$tag_item)
TkcLine.new(cvs, '6.33c','1c','6.33c','4c', 'arrow'=>'both', 'tags'=>$tag_item)
TkcLine.new(cvs, '5c','6c','9c','6c','9c','1c','8c','1c','8c','4.8c','8.8c',
            '4.8c','8.8c','1.2c','8.2c','1.2c','8.2c','4.6c','8.6c','4.6c',
            '8.6c','1.4c','8.4c','1.4c','8.4c','4.4c',
            'width'=>3, 'fill'=>red, 'tags'=>$tag_item )
TkcLine.new(cvs, '1c','5c','7c','5c','7c','7c','9c','7c', 'width'=>'.5c',
            'stipple'=>'@'+[$demo_dir,'..',
                            'images','gray25.xbm'].join(File::Separator),
            'arrow'=>'both', 'arrowshape'=>'15 15 7', 'tags'=>$tag_item )
TkcLine.new(cvs, '1c','7c','1.75c','5.8c','2.5c','7c','3.25c','5.8c','4c','7c',
            'width'=>'.5c', 'capstyle'=>'round', 'join'=>'round',
            'tags'=>$tag_item )

TkcText.new(cvs, '15c', '.2c',
            'text'=>'Curves (smoothed lines)', 'anchor'=>'n')
TkcLine.new(cvs, '11c','4c','11.5c','1c','13.5c','1c','14c','4c',
            'smooth'=>'on', 'fill'=>blue, 'tags'=>$tag_item )
TkcLine.new(cvs, '15.5c','1c','19.5c','1.5c','15.5c','4.5c','19.5c','4c',
            'smooth'=>'on', 'arrow'=>'both', 'width'=>3, 'tags'=>$tag_item )
TkcLine.new(cvs, '12c','6c','13.5c','4.5c','16.5c','7.5c','18c','6c',
            '16.5c','4.5c','13.5c','7.5c','12c','6c',
            'smooth'=>'on', 'width'=>'3m', 'capstyle'=>'round',
            'stipple'=>'@'+[$demo_dir, '..',
                            'images', 'gray25.xbm'].join(File::Separator),
            'fill'=>red, 'tags'=>$tag_item )

TkcText.new(cvs, '25c', '.2c', 'text'=>'Polygons', 'anchor'=>'n')
TkcPolygon.new(cvs, '21c','1.0c','22.5c','1.75c','24c','1.0c','23.25c','2.5c',
               '24c','4.0c','22.5c','3.25c','21c','4.0c','21.75c','2.5c',
               'fill'=>'green', 'outline'=>'black', 'width'=>4,
               'tags'=>$tag_item )
TkcPolygon.new(cvs, '25c','4c','25c','4c','25c','1c','26c','1c','27c','4c',
               '28c','1c','29c','1c','29c','4c','29c','4c',
               'fill'=>red, 'smooth'=>'on', 'tags'=> $tag_item)
TkcPolygon.new(cvs, '22c','4.5c','25c','4.5c','25c','6.75c','28c','6.75c',
               '28c','5.25c','24c','5.25c','24c','6.0c','26c','6c','26c',
               '7.5c','22c','7.5c',
               'stipple'=>'@' + [$demo_dir, '..',
                                 'images', 'gray25.xbm'].join(File::Separator),
               'outline'=>'black', 'tags'=>$tag_item )

TkcText.new(cvs, '5c', '8.2c', 'text'=>'Rectangles', 'anchor'=>'n')
TkcRectangle.new(cvs, '1c','9.5c','4c','12.5c',
                 'outline'=>red, 'width'=>'3m', 'tags'=>$tag_item)
TkcRectangle.new(cvs, '0.5c','13.5c','4.5c','15.5c',
                 'fill'=>green, 'tags'=>$tag_item )
TkcRectangle.new(cvs, '6c','10c','9c','15c', 'outline'=>'',
                 'stipple'=>'@'+[$demo_dir,'..',
                                 'images','gray25.xbm'].join(File::Separator),
                 'fill'=>blue, 'tags'=>$tag_item )

TkcText.new(cvs, '15c', '8.2c', 'text'=>'Ovals', 'anchor'=>'n')
TkcOval.new(cvs, '11c','9.5c','14c','12.5c',
                 'outline'=>red, 'width'=>'3m', 'tags'=>$tag_item)
TkcOval.new(cvs, '10.5c','13.5c','14.5c','15.5c',
                 'fill'=>green, 'tags'=>$tag_item )
TkcOval.new(cvs, '16c','10c','19c','15c', 'outline'=>'',
                 'stipple'=>'@'+[$demo_dir,'..',
                                 'images','gray25.xbm'].join(File::Separator),
                 'fill'=>blue, 'tags'=>$tag_item )

TkcText.new(cvs, '25c', '8.2c', 'text'=>'Text', 'anchor'=>'n')
TkcRectangle.new(cvs, '22.4c','8.9c','22.6c','9.1c')
TkcText.new(cvs, '22.5c', '9c', 'anchor'=>'n', 'font'=>font1, 'width'=>'4c',
            'text'=>'A short string of text, word-wrapped, justified left, and anchored north (at the top).  The rectangles show the anchor points for each piece of text.', 'tags'=>$tag_item )
TkcRectangle.new(cvs, '25.4c','10.9c','25.6c','11.1c')
TkcText.new(cvs, '25.5c', '11c', 'anchor'=>'w', 'font'=>font1, 'fill'=>blue,
            'text'=>"Several lines,\n each centered\nindividually,\nand all anchored\nat the left edge.", 'justify'=>'center', 'tags'=>$tag_item )
TkcRectangle.new(cvs, '24.9c','13.9c','25.1c','14.1c')
if $tk_version =~ /^4\.[01]/
  TkcText.new(cvs, '25c', '14c', 'anchor'=>'c', 'font'=>font2, 'fill'=>red,
              'stipple'=>'@' + [$demo_dir, '..',
                                'images', 'grey.5'].join(File::Separator),
              'text'=>'Stippled characters', 'tags'=>$tag_item )
else
  TkcText.new(cvs, '25c', '14c', 'anchor'=>'c', 'font'=>font2, 'fill'=>red,
              'stipple'=>'gray50', 'text'=>'Stippled characters',
              'tags'=>$tag_item )
end

TkcText.new(cvs, '5c', '16.2c', 'text'=>'Arcs', 'anchor'=>'n')
TkcArc.new(cvs, '0.5c','17c','7c','20c', 'fill'=>green, 'outline'=>'black',
           'start'=>45, 'extent'=>270, 'style'=>'pieslice', 'tags'=>$tag_item)
#TkcArc.new(cvs, '6.5c','17c','9.5c','20c', 'width'=>'4m', 'style'=>'arc',
#          'outline'=>blue, 'start'=>135, 'extent'=>270,
#          'outlinestipple'=>'@' + ['images', 'grey.25'].join(File::Separator),
#          'tags'=>$tag_item)
TkcArc.new(cvs, '6.5c','17c','9.5c','20c', 'width'=>'4m', 'style'=>'arc',
           'outline'=>blue, 'start'=>135, 'extent'=>270,
           'outlinestipple'=>'@'+[$demo_dir, '..',
                                  'images','gray25.xbm'].join(File::Separator),
           'tags'=>$tag_item)
TkcArc.new(cvs, '0.5c','20c','9.5c','24c', 'width'=>'4m', 'style'=>'pieslice',
           'fill'=>'', 'outline'=>red, 'start'=>225, 'extent'=>90,
           'tags'=>$tag_item)
TkcArc.new(cvs, '5.5c','20.5c','9.5c','23.5c', 'width'=>'4m', 'style'=>'chord',
           'fill'=>blue, 'outline'=>'', 'start'=>45, 'extent'=>270,
           'tags'=>$tag_item)

TkcText.new(cvs, '15c', '16.2c', 'text'=>'Bitmaps', 'anchor'=>'n')
#TkcBitmap.new(cvs, '13c','20c',
#             'bitmap'=>'@' + ['images', 'face'].join(File::Separator),
#             'tags'=>$tag_item)
TkcBitmap.new(cvs, '13c','20c',
              'bitmap'=>'@' + [$demo_dir, '..',
                               'images', 'face.xbm'].join(File::Separator),
              'tags'=>$tag_item)
#TkcBitmap.new(cvs, '17c','18.5c',
#             'bitmap'=>'@' + ['images', 'noletters'].join(File::Separator),
#             'tags'=>$tag_item)
TkcBitmap.new(cvs, '17c','18.5c',
              'bitmap'=>'@' + [$demo_dir, '..',
                               'images', 'noletter.xbm'].join(File::Separator),
              'tags'=>$tag_item)
#TkcBitmap.new(cvs, '17c','21.5c',
#             'bitmap'=>'@' + ['images', 'letters'].join(File::Separator),
#             'tags'=>$tag_item)
#
TkcBitmap.new(cvs, '17c','21.5c') {
  bitmap '@' + [$demo_dir, '..', 'images', 'letters.xbm'].join(File::Separator)
  tags $tag_item
}
#TkcBitmap.new(cvs, '17c','21.5c') {
#  bitmap '@' + ['images', 'letters'].join(File::Separator)
#  tags $tag_item
#}

TkcText.new(cvs, '25c', '16.2c', 'text'=>'Windows', 'anchor'=>'n')
TkButton.new(cvs) {|b|
  text 'Press Me'
  command proc{butPress cvs, red}
  TkcWindow.new(cvs, '21c','18c',
                'window'=>b, 'anchor'=>'nw', 'tags'=>$tag_item)
}
TkEntry.new(cvs, 'width'=>20, 'relief'=>'sunken') {|e|
  insert 'end', 'Edit this text'
  TkcWindow.new(cvs, '21c','21c',
                'window'=>e, 'anchor'=>'nw', 'tags'=>$tag_item)
}
TkScale.new(cvs, 'from'=>0, 'to'=>100, 'length'=>'6c', 'sliderlength'=>'.4c',
            'width'=>'.5c', 'tickinterval'=>0 ) {|scl|
  TkcWindow.new(cvs, '28.5c','17.5c',
                'window'=>scl, 'anchor'=>'n', 'tags'=>$tag_item)
}
TkcText.new(cvs, '21c', '17.9c', 'text'=>'Button:', 'anchor'=>'sw')
TkcText.new(cvs, '21c', '20.9c', 'text'=>'Entry:', 'anchor'=>'sw')
TkcText.new(cvs, '28.5c', '17.4c', 'text'=>'Scale:', 'anchor'=>'s')

# Set up event bindings for canvas:
cvs.itembind($tag_item, 'Any-Enter', proc{itemEnter cvs})
cvs.itembind($tag_item, 'Any-Leave', proc{itemLeave cvs})
cvs.bind('2', proc{|x,y| cvs.scan_mark x,y}, '%x %y')
cvs.bind('B2-Motion', proc{|x,y| cvs.scan_dragto x,y}, '%x %y')
cvs.bind('3', proc{|x,y| itemMark cvs,x,y}, '%x %y')
cvs.bind('B3-Motion', proc{|x,y| itemStroke cvs,x,y}, '%x %y')
cvs.bind('Control-f', proc{itemsUnderArea cvs})
cvs.bind('1', proc{|x,y| itemStartDrag cvs,x,y}, '%x %y')
cvs.bind('B1-Motion', proc{|x,y| itemDrag cvs,x,y}, '%x %y')
# Utility methods for highlighting the item under the pointer

$restoreCmd = nil
def itemEnter (c)
  if TkWinfo.depth(c).to_i == 1
    $restoreCmd = nil
    return
  end
  type = c.itemtype('current')
  if type == TkcWindow
    $restoreCmd = nil
    return
  end
  if type == TkcBitmap
    bg = (c.itemconfiginfo('current', 'background'))[4]
    $restoreCmd = proc{c.itemconfigure 'current', 'background', bg}
    c.itemconfigure 'current', 'background', 'SteelBlue2'
    return
  end
  fill = (c.itemconfiginfo('current', 'fill'))[4]
  if (type == TkcRectangle || type == TkcOval || type == TkcArc) && fill == []
    outline = (c.itemconfiginfo('current', 'outline'))[4]
    $restoreCmd = proc{c.itemconfigure 'current', 'outline', outline}
    c.itemconfigure 'current', 'outline', 'SteelBlue2'
  else
    $restoreCmd = proc{c.itemconfigure 'current', 'fill', fill}
    c.itemconfigure 'current', 'fill', 'SteelBlue2'
  end
end

def itemLeave(c)
  $restoreCmd.call if $restoreCmd
end

# Utility methods for stroking out a rectangle and printing what's
# underneath the rectangle's area.

def itemMark(c,x,y)
  $areaX1 = c.canvasx(x)
  $areaY1 = c.canvasy(y)
  c.delete 'area'
end

def itemStroke(c,x,y)
  x = c.canvasx(x)
  y = c.canvasy(y)
  if $areaX1 != x && $areaY1 != y
    c.delete 'area'
    c.addtag_withtag 'area', TkcRectangle.new(c, $areaX1, $areaY1, x, y,
                                              '-outline', 'black')
    $areaX2 = x
    $areaY2 = y
  end
end

def itemsUnderArea(c)
  area = c.find_withtag('area')
  items = []
  c.find_enclosed($areaX1,$areaY1,$areaX2,$areaY2).each{|i|
    items.push(i) if i.gettags.include?($tag_item)
  }
  print "Items enclosed by area: #{items.inspect}\n"; STDOUT.flush
  items.clear
  c.find_overlapping($areaX1,$areaY1,$areaX2,$areaY2).each{|i|
    items.push(i) if i.gettags.include?($tag_item)
  }
  print "Items overlapping area: #{items.inspect}\n"; STDOUT.flush
end

$areaX1 = 0
$areaY1 = 0
$areaX2 = 0
$areaY2 = 0

# Utility methods to support dragging of items.

def itemStartDrag(c,x,y)
  $lastX = c.canvasx(x)
  $lastY = c.canvasy(y)
end

def itemDrag(c,x,y)
  x = c.canvasx(x)
  y = c.canvasy(y)
  c.move 'current', x - $lastX, y - $lastY
  $lastX = x
  $lastY = y
end

# Method that's invoked when the button embedded in the canvas
# is invoked.

def butPress(w,color)
  i = TkcText.new(w, '25c', '18.1c',
                  'text'=>'Ouch!!', 'fill'=>color, 'anchor'=>'n')
  Tk.after(500, proc{w.delete i})
end
