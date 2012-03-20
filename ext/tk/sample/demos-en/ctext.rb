# ctext.rb
#
# This demonstration script creates a canvas widget with a text
# item that can be edited and reconfigured in various ways.
#
# Canvas Text widget demo (called by 'widget')
#

# toplevel widget
if defined?($ctext_demo) && $ctext_demo
  $ctext_demo.destroy
  $ctext_demo = nil
end

# demo toplevel widget
$ctext_demo = TkToplevel.new {|w|
  title("Canvas Text Demonstration")
  iconname("Text")
  positionWindow(w)
}

base_frame = TkFrame.new($ctext_demo).pack(:fill=>:both, :expand=>true)

# label
TkLabel.new(base_frame, 'font'=>$font, 'wraplength'=>'5i', 'justify'=>'left',
            'text'=>"This window displays a string of text to demonstrate the text facilities of canvas widgets.  You can click in the boxes to adjust the position of the text relative to its positioning point or change its justification.  The text also supports the following simple bindings for editing:
  1. You can point, click, and type.
  2. You can also select with button 1.
  3. You can copy the selection to the mouse position with button 2.
  4. Backspace and Control+h delete the selection if there is one;
     otherwise they delete the character just before the insertion cursor.
  5. Delete deletes the selection if there is one; otherwise it deletes
     the character just after the insertion cursor."){
  pack('side'=>'top')
}

# frame
$ctext_buttons = TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    text 'Dismiss'
    command proc{
      tmppath = $ctext_demo
      $ctext_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'Show Code'
    command proc{showCode 'ctext'}
  }.pack('side'=>'left', 'expand'=>'yes')
}
$ctext_buttons.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# canvas
$ctext_canvas = TkCanvas.new(base_frame, 'relief'=>'flat',
                             'borderwidth'=>0, 'width'=>500, 'height'=>350)
$ctext_canvas.pack('side'=>'top', 'expand'=>'yes', 'fill'=>'both')

# font
if $tk_version =~ /^4.*/
  textFont = '-*-Helvetica-Medium-R-Normal--*-240-*-*-*-*-*-*'
else
  textFont = 'Helvetica 24'
end

# canvas
TkcRectangle.new($ctext_canvas, 245, 195, 255, 205,
                 'outline'=>'black', 'fill'=>'red')

ctag_text_param = {
  'text'=>"This is just a string of text to demonstrate the text facilities of canvas widgets. Bindings have been been defined to support editing (see above).",
  'width'=>440, 'anchor'=>'n', 'justify'=>'left'
}
if $tk_version =~ /^4.*/
  ctag_text_param['font'] = '-*-Helvetica-Medium-R-Normal--*-240-*-*-*-*-*-*'
else
  ctag_text_param['font'] = 'Helvetica 24'
end

$ctag_text = TkcTag.new($ctext_canvas)
$ctag_text.withtag(TkcText.new($ctext_canvas, 250, 200, ctag_text_param))

$ctag_text.bind('1', proc{|x,y| textB1Press $ctext_canvas,x,y}, "%x %y")
$ctag_text.bind('B1-Motion', proc{|x,y| textB1Move $ctext_canvas,x,y}, "%x %y")
$ctag_text.bind('Shift-1',
        proc{|x,y| $ctext_canvas.seleect_adjust 'current', "@#{x},#{y}"},
        "%x %y")
$ctag_text.bind('Shift-B1-Motion',
                proc{|x,y| textB1Move $ctext_canvas,x,y}, "%x %y")
$ctag_text.bind('KeyPress', proc{|a| textInsert $ctext_canvas,a}, "%A")
$ctag_text.bind('Return', proc{textInsert $ctext_canvas,"\n"})
$ctag_text.bind('Control-h', proc{textBs $ctext_canvas})
$ctag_text.bind('BackSpace', proc{textBs $ctext_canvas})
$ctag_text.bind('Delete', proc{textDel $ctext_canvas})
$ctag_text.bind('2', proc{|x,y| textPaste $ctext_canvas, "@#{x},#{y}"},
                "%x %y")

# Next, create some items that allow the text's anchor position
# to be edited.

def mkTextConfig(w,x,y,option,value,color)
  item = TkcRectangle.new(w, x, y, x+30, y+30,
                          'outline'=>'black', 'fill'=>color, 'width'=>1)
  item.bind('1', proc{$ctag_text.configure option, value})
  w.addtag_withtag('config', item)
end

x = 50
y = 50
color = 'LightSkyBlue1'
mkTextConfig $ctext_canvas, x, y, 'anchor', 'se', color
mkTextConfig $ctext_canvas, x+30, y, 'anchor', 's', color
mkTextConfig $ctext_canvas, x+60, y, 'anchor', 'sw', color
mkTextConfig $ctext_canvas, x, y+30, 'anchor', 'e', color
mkTextConfig $ctext_canvas, x+30, y+30, 'anchor', 'center', color
mkTextConfig $ctext_canvas, x+60, y+30, 'anchor', 'w', color
mkTextConfig $ctext_canvas, x, y+60, 'anchor', 'ne', color
mkTextConfig $ctext_canvas, x+30, y+60, 'anchor', 'n', color
mkTextConfig $ctext_canvas, x+60, y+60, 'anchor', 'nw', color
item = TkcRectangle.new($ctext_canvas, x+40, y+40, x+50, y+50,
                        'outline'=>'black', 'fill'=>'red')
item.bind('1', proc{$ctag_text.configure 'anchor', 'center'})
if $tk_version =~ /^4.*/
  TkcText.new($ctext_canvas, x+45, y-5, 'text'=>'Text Position',
              'font'=>'-*-times-medium-r-normal--*-240-*-*-*-*-*-*',
              'anchor'=>'s', 'fill'=>'brown')
else
  TkcText.new($ctext_canvas, x+45, y-5, 'text'=>'Text Position',
              'font'=>'Times 24', 'anchor'=>'s', 'fill'=>'brown')
end

# Lastly, create some items that allow the text's justification to be
# changed.

x = 350
y = 50
color = 'SeaGreen2'
mkTextConfig $ctext_canvas, x, y, 'justify', 'left', color
mkTextConfig $ctext_canvas, x+30, y, 'justify', 'center', color
mkTextConfig $ctext_canvas, x+60, y, 'justify', 'right', color
if $tk_version =~ /^4.*/
  TkcText.new($ctext_canvas, x+45, y-5, 'text'=>'Justification',
              'font'=>'-*-times-medium-r-normal--*-240-*-*-*-*-*-*',
              'anchor'=>'s', 'fill'=>'brown')
else
  TkcText.new($ctext_canvas, x+45, y-5, 'text'=>'Justification',
              'font'=>'Times 24', 'anchor'=>'s', 'fill'=>'brown')
end

$ctext_canvas.itembind('config', 'Enter', proc{textEnter $ctext_canvas})
$ctext_canvas.itembind('config', 'Leave',
                       proc{$ctext_canvas\
                             .itemconfigure('current',
                                            'fill'=>$textConfigFill)})

$textConfigFill = ''

def textEnter(w)
  $textConfigFill = (w.itemconfiginfo 'current', 'fill')[4]
  w.itemconfigure 'current', 'fill', 'black'
end

def textInsert(w, string)
  return if string == ""
  begin
    $ctag_text.dchars 'sel.first', 'sel.last'
  rescue
  end
  $ctag_text.insert 'insert', string
end

def textPaste(w, pos)
  begin
    $ctag_text.insert pos, TkSelection.get
  rescue
  end
end

def textB1Press(w,x,y)
  w.icursor 'current', "@#{x},#{y}"
  w.itemfocus 'current'
  w.focus
  w.select_from 'current', "@#{x},#{y}"
end

def textB1Move(w,x,y)
  w.select_to 'current', "@#{x},#{y}"
end

def textBs(w)
  begin
    $ctag_text.dchars 'sel.first', 'sel.last'
  rescue
    char = $ctag_text.index('insert').to_i - 1
    $ctag_text.dchars(char) if char >= 0
  end
end

def textDel(w)
  begin
    $ctag_text.dchars 'sel.first', 'sel.last'
  rescue
    $ctag_text.dchars 'insert'
  end
end

