# states.rb
#
# This demonstration script creates a listbox widget that displays
# the names of the 50 states in the United States of America.
#
# listbox widget demo 'states' (called by 'widget')
#

# toplevel widget
if defined?($states_demo) && $states_demo
  $states_demo.destroy
  $states_demo = nil
end

# demo toplevel widget
$states_demo = TkToplevel.new {|w|
  title("Listbox Demonstration (states)")
  iconname("states")
  positionWindow(w)
}

base_frame = TkFrame.new($states_demo).pack(:fill=>:both, :expand=>true)

# label
msg = TkLabel.new(base_frame) {
  font $font
  wraplength '4i'
  justify 'left'
  text "A listbox containing the 50 states is displayed below, along with a scrollbar.  You can scan the list either using the scrollbar or by scanning.  To scan, press button 2 in the widget and drag up or down."
}
msg.pack('side'=>'top')

# frame
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    text 'Dismiss'
    command proc{
      tmppath = $states_demo
      $states_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'Show Code'
    command proc{showCode 'states'}
  }.pack('side'=>'left', 'expand'=>'yes')

}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# frame
states_lbox = nil
TkFrame.new(base_frame, 'borderwidth'=>'.5c') {|w|
  s = TkScrollbar.new(w)
  states_lbox = TkListbox.new(w) {
    setgrid 1
    height 12
    yscrollcommand proc{|first,last| s.set first,last}
  }
  s.command(proc{|*args| states_lbox.yview(*args)})
  s.pack('side'=>'right', 'fill'=>'y')
  states_lbox.pack('side'=>'left', 'expand'=>1, 'fill'=>'both')
}.pack('side'=>'top', 'expand'=>'yes', 'fill'=>'y')

ins_data = [
  'Alabama', 'Alaska', 'Arizona', 'Arkansas', 'California',
  'Colorado', 'Connecticut', 'Delaware', 'Florida', 'Georgia',
  'Hawaii', 'Idaho', 'Illinois',
  'Indiana', 'Iowa', 'Kansas', 'Kentucky', 'Louisiana', 'Maine', 'Maryland',
  'Massachusetts', 'Michigan', 'Minnesota', 'Mississippi', 'Missouri',
  'Montana', 'Nebraska', 'Nevada', 'New_Hampshire', 'New_Jersey', 'New_Mexico',
  'New_York', 'North_Carolina', 'North_Dakota',
  'Ohio', 'Oklahoma', 'Oregon', 'Pennsylvania', 'Rhode_Island',
  'South_Carolina', 'South_Dakota',
  'Tennessee', 'Texas', 'Utah', 'Vermont', 'Virginia', 'Washington',
  'West_Virginia', 'Wisconsin', 'Wyoming'
]

states_lbox.insert(0, *ins_data)

