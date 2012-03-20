#
# entry (no scrollbars) widget demo (called by 'widget')
#

# toplevel widget
if defined?($entry1_demo) && $entry1_demo
  $entry1_demo.destroy
  $entry1_demo = nil
end

# demo toplevel widget
$entry1_demo = TkToplevel.new {|w|
  title("Entry Demonstration (no scrollbars)")
  iconname("entry1")
  positionWindow(w)
}

base_frame = TkFrame.new($entry1_demo).pack(:fill=>:both, :expand=>true)

# label
msg = TkLabel.new(base_frame) {
  font $font
  wraplength '5i'
  justify 'left'
  text "Three different entries are displayed below.  You can add characters by pointing, clicking and typing.  The normal Motif editing characters are supported, along with many Emacs bindings.  For example, Backspace and Control-h delete the character to the left of the insertion cursor and Delete and Control-d delete the chararacter to the right of the insertion cursor.  For entries that are too large to fit in the window all at once, you can scan through the entries by dragging with mouse button2 pressed."
}
msg.pack('side'=>'top')

# frame
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    text 'Dismiss'
    command proc{
      tmppath = $entry1_demo
      $entry1_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'Show Code'
    command proc{showCode 'entry1'}
  }.pack('side'=>'left', 'expand'=>'yes')
}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

#
e1 = TkEntry.new(base_frame, 'relief'=>'sunken')
e2 = TkEntry.new(base_frame, 'relief'=>'sunken')
e3 = TkEntry.new(base_frame, 'relief'=>'sunken')
[e1,e2,e3].each{|w| w.pack('side'=>'top', 'padx'=>10, 'pady'=>5, 'fill'=>'x')}

#
e1.insert(0, 'Initial value')
e2.insert('end', "This entry contains a long value, much too long ")
e2.insert('end', "to fit in the window at one time, so long in fact ")
e2.insert('end', "that you'll have to scan or scroll to see the end.")
e2.insert('end', "")

