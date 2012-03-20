# radio.rb
#
# This demonstration script creates a toplevel window containing
# several radiobutton widgets.
#
# radiobutton widget demo (called by 'widget')
#

# toplevel widget
if defined?($radio_demo) && $radio_demo
  $radio_demo.destroy
  $radio_demo = nil
end

# demo toplevel widget
$radio_demo = TkToplevel.new {|w|
  title("Radiobutton Demonstration")
  iconname("radio")
  positionWindow(w)
}

base_frame = TkFrame.new($radio_demo).pack(:fill=>:both, :expand=>true)

# label
msg = TkLabel.new(base_frame) {
  font $font
  wraplength '4i'
  justify 'left'
  text "Two groups of radiobuttons are displayed below.  If you click on a button then the button will become selected exclusively among all the buttons in its group.  A Tcl variable is associated with each group to indicate which of the group's buttons is selected.  Click the \"See Variables\" button to see the current values of the variables."
}
msg.pack('side'=>'top')

#
size = TkVariable.new
color = TkVariable.new

# frame
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    text 'Dismiss'
    command proc{
      tmppath = $radio_demo
      $radio_demo = nil
      $showVarsWin[tmppath.path] = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'Show Code'
    command proc{showCode 'radio'}
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'See Variables'
    command proc{
      showVars(base_frame, ['size', size], ['color', color])
    }
  }.pack('side'=>'left', 'expand'=>'yes')
}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# frame
f_left = TkFrame.new(base_frame)
f_right = TkFrame.new(base_frame)
f_left.pack('side'=>'left', 'expand'=>'yes', 'padx'=>'.5c', 'pady'=>'.5c')
f_right.pack('side'=>'left', 'expand'=>'yes', 'padx'=>'.5c', 'pady'=>'.5c')

# radiobutton
[10, 12, 18, 24].each {|sz|
  TkRadioButton.new(f_left) {
    text "Point Size #{sz}"
    variable size
    relief 'flat'
    value sz
  }.pack('side'=>'top', 'pady'=>2, 'anchor'=>'w')
}

['Red', 'Green', 'Blue', 'Yellow', 'Orange', 'Purple'].each {|col|
  TkRadioButton.new(f_right) {
    text col
    variable color
    relief 'flat'
    value col.downcase
  }.pack('side'=>'top', 'pady'=>2, 'anchor'=>'w')
}

