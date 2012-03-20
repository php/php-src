# entry2.rb
#
# This demonstration script is the same as the entry1.tcl script
# except that it creates scrollbars for the entries.
#
# entry (with scrollbars) widget demo (called by 'widget')
#

# toplevel widget
if defined?($entry2_demo) && $entry2_demo
  $entry2_demo.destroy
  $entry2_demo = nil
end

# demo toplevel widget
$entry2_demo = TkToplevel.new {|w|
  title("Entry Demonstration (with scrollbars)")
  iconname("entry2")
  positionWindow(w)
}

base_frame = TkFrame.new($entry2_demo).pack(:fill=>:both, :expand=>true)

# label
msg = TkLabel.new(base_frame) {
  font $font
  wraplength '5i'
  justify 'left'
  text "Three different entries are displayed below, with a scrollbar for each entry.  You can add characters by pointing, clicking and typing.  The normal Motif editing characters are supported, along with many Emacs bindings.  For example, Backspace and Control-h delete the character to the left of the insertion cursor and Delete and Control-d delete the chararacter to the right of the insertion cursor.  For entries that are too large to fit in the window all at once, you can scan through the entries with the scrollbars, or by dragging with mouse button2 pressed."
}
msg.pack('side'=>'top')

# frame
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    text 'Dismiss'
    command proc{
      tmppath = $entry2_demo
      $entry2_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'Show Code'
    command proc{showCode 'entry2'}
  }.pack('side'=>'left', 'expand'=>'yes')
}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# frame
TkFrame.new(base_frame, 'borderwidth'=>10) {|w|
  # entry 1
  s1 = TkScrollbar.new(w, 'relief'=>'sunken', 'orient'=>'horiz')
  e1 = TkEntry.new(w, 'relief'=>'sunken') {
    xscrollcommand proc{|first,last| s1.set first,last}
  }
  s1.command(proc{|*args| e1.xview(*args)})
  e1.pack('side'=>'top', 'fill'=>'x')
  s1.pack('side'=>'top', 'fill'=>'x')

  # spacer
  TkFrame.new(w, 'width'=>20, 'height'=>10).pack('side'=>'top', 'fill'=>'x')

  # entry 2
  s2 = TkScrollbar.new(w, 'relief'=>'sunken', 'orient'=>'horiz')
  e2 = TkEntry.new(w, 'relief'=>'sunken') {
    xscrollcommand proc{|first,last| s2.set first,last}
  }
  s2.command(proc{|*args| e2.xview(*args)})
  e2.pack('side'=>'top', 'fill'=>'x')
  s2.pack('side'=>'top', 'fill'=>'x')

  # spacer
  TkFrame.new(w, 'width'=>20, 'height'=>10).pack('side'=>'top', 'fill'=>'x')

  # entry 3
  s3 = TkScrollbar.new(w, 'relief'=>'sunken', 'orient'=>'horiz')
  e3 = TkEntry.new(w, 'relief'=>'sunken') {
    xscrollcommand proc{|first,last| s3.set first,last}
  }
  s3.command(proc{|*args| e3.xview(*args)})
  e3.pack('side'=>'top', 'fill'=>'x')
  s3.pack('side'=>'top', 'fill'=>'x')

  #
  e1.insert(0, 'Initial value')
  e2.insert('end', "This entry contains a long value, much too long ")
  e2.insert('end', "to fit in the window at one time, so long in fact ")
  e2.insert('end', "that you'll have to scan or scroll to see the end.")
  e2.insert('end', "")

}.pack('side'=>'top', 'fill'=>'x', 'expand'=>'yes')

