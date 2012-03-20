# check.rb
#
# This demonstration script creates a toplevel window containing
# several checkbuttons.
#
# checkbutton widget demo (called by 'widget')
#

# toplevel widget
if defined?($check_demo) && $check_demo
  $check_demo.destroy
  $check_demo = nil
end

# demo toplevel widget
$check_demo = TkToplevel.new {|w|
  title("Checkbutton Demonstration")
  iconname("check")
  positionWindow(w)
}

base_frame = TkFrame.new($check_demo).pack(:fill=>:both, :expand=>true)

# label
msg = TkLabel.new(base_frame) {
  font $font
  wraplength '4i'
  justify 'left'
  text "Three checkbuttons are displayed below.  If you click on a button, it will toggle the button's selection state and set a Tcl variable to a value indicating the state of the checkbutton.  Click the \"See Variables\" button to see the current values of the variables."
}
msg.pack('side'=>'top')

#
wipers = TkVariable.new(0)
brakes = TkVariable.new(0)
sober  = TkVariable.new(0)

# frame
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    text 'Dismiss'
    command proc{
      tmppath = $check_demo
      $check_demo = nil
      $showVarsWin[tmppath.path] = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'Show Code'
    command proc{showCode 'check'}
  }.pack('side'=>'left', 'expand'=>'yes')


  TkButton.new(frame) {
    text 'See Variables'
    command proc{
      showVars(base_frame,
               ['wipers', wipers], ['brakes', brakes], ['sober', sober])
    }
  }.pack('side'=>'left', 'expand'=>'yes')

}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')


# checkbutton
[ TkCheckButton.new(base_frame, 'text'=>'Wipers  OK', 'variable'=>wipers),
  TkCheckButton.new(base_frame, 'text'=>'Brakes  OK', 'variable'=>brakes),
  TkCheckButton.new(base_frame, 'text'=>'Driver Sober', 'variable'=>sober)
].each{|w| w.relief('flat'); w.pack('side'=>'top', 'pady'=>2, 'anchor'=>'w')}

