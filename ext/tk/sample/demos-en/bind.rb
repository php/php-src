# bind.rb
#
# This demonstration script creates a text widget with bindings set
# up for hypertext-like effects.
#
# text (tag bindings) widget demo (called by 'widget')
#

# toplevel widget
if defined?($bind_demo) && $bind_demo
  $bind_demo.destroy
  $bind_demo = nil
end

# demo toplevel widget
$bind_demo = TkToplevel.new {|w|
  title("Text Demonstration - Tag Bindings")
  iconname("bind")
  positionWindow(w)
}

base_frame = TkFrame.new($bind_demo).pack(:fill=>:both, :expand=>true)

# frame
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    text 'Dismiss'
    command proc{
      tmppath = $bind_demo
      $bind_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'Show Code'
    command proc{showCode 'bind'}
  }.pack('side'=>'left', 'expand'=>'yes')
}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# bind
def tag_binding_for_bind_demo(tag, enter_style, leave_style)
  tag.bind('Any-Enter', proc{tag.configure enter_style})
  tag.bind('Any-Leave', proc{tag.configure leave_style})
end

# text
txt = TkText.new(base_frame){|t|
  #
  setgrid 'true'
  #width  60
  #height 24
  font $font
  wrap 'word'
  TkScrollbar.new(base_frame) {|s|
    pack('side'=>'right', 'fill'=>'y')
    command proc{|*args| t.yview(*args)}
    t.yscrollcommand proc{|first,last| s.set first,last}
  }
  pack('expand'=>'yes', 'fill'=>'both')

  #
  if TkWinfo.depth($root).to_i > 1
    tagstyle_bold = {'background'=>'#43ce80', 'relief'=>'raised',
                     'borderwidth'=>1}
    tagstyle_normal = {'background'=>'', 'relief'=>'flat'}
  else
    tagstyle_bold = {'foreground'=>'white', 'background'=>'black'}
    tagstyle_normal = {'foreground'=>'', 'background'=>''}
  end

  # insert text
  insert 'insert', "The same tag mechanism that controls display styles in text widgets can also be used to associate Tcl commands with regions of text, so that mouse or keyboard actions on the text cause particular Tcl commands to be invoked.  For example, in the text below the descriptions of the canvas demonstrations have been tagged.  When you move the mouse over a demo description the description lights up, and when you press button 1 over a description then that particular demonstration is invoked.

"
  insert('end', '1. Samples of all the different types of items that can be created in canvas widgets.', (d1 = TkTextTag.new(t)) )
  insert('end', "\n\n")
  insert('end', '2. A simple two-dimensional plot that allows you to adjust the positions of the data points.', (d2 = TkTextTag.new(t)) )
  insert('end', "\n\n")
  insert('end', '3. Anchoring and justification modes for text items.',
         (d3 = TkTextTag.new(t)) )
  insert('end', "\n\n")
  insert('end', '4. An editor for arrow-head shapes for line items.',
         (d4 = TkTextTag.new(t)) )
  insert('end', "\n\n")
  insert('end', '5. A ruler with facilities for editing tab stops.',
         (d5 = TkTextTag.new(t)) )
  insert('end', "\n\n")
  insert('end',
         '6. A grid that demonstrates how canvases can be scrolled.',
         (d6 = TkTextTag.new(t)) )

  # binding
  [d1, d2, d3, d4, d5, d6].each{|tag|
    tag_binding_for_bind_demo(tag, tagstyle_bold, tagstyle_normal)
  }
  d1.bind('1',
          proc{
            eval_samplecode(`cat #{[$demo_dir,'items.rb'].join(File::Separator)}`, 'items.rb')
          })
  d2.bind('1',
          proc{
            eval_samplecode(`cat #{[$demo_dir,'plot.rb'].join(File::Separator)}`, 'plot.rb')
          })
  d3.bind('1',
          proc{
            eval_samplecode(`cat #{[$demo_dir,'ctext.rb'].join(File::Separator)}`, 'ctext.rb')
          })
  d4.bind('1',
          proc{
            eval_samplecode(`cat #{[$demo_dir,'arrow.rb'].join(File::Separator)}`, 'arrow.rb')
          })
  d5.bind('1',
          proc{
            eval_samplecode(`cat #{[$demo_dir,'ruler.rb'].join(File::Separator)}`, 'ruler.rb')
          })
  d6.bind('1',
          proc{
            eval_samplecode(`cat #{[$demo_dir,'cscroll.rb'].join(File::Separator)}`, 'cscroll.rb')
          })

  TkTextMarkInsert.new(t, '0.0')
  configure('state','disabled')
}

txt.width  60
txt.height 24
