# label.rb
#
# This demonstration script creates a toplevel window containing
# several label widgets.
#
# label widget demo (called by 'widget')
#

# toplevel widget
if defined?($label_demo) && $label_demo
  $label_demo.destroy
  $label_demo = nil
end

# demo toplevel widget
$label_demo = TkToplevel.new {|w|
  title("Label Demonstration")
  iconname("label")
  positionWindow(w)
}

base_frame = TkFrame.new($label_demo).pack(:fill=>:both, :expand=>true)

# label
msg = TkLabel.new(base_frame) {
  font $font
  wraplength '4i'
  justify 'left'
  text "Five labels are displayed below: three textual ones on the left, and a bitmap label and a text label on the right.  Labels are pretty boring because you can't do anything with them."
}
msg.pack('side'=>'top')

# frame
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    text 'Dismiss'
    command proc{
      tmppath = $label_demo
      $label_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'See Code'
    command proc{showCode 'label'}
  }.pack('side'=>'left', 'expand'=>'yes')

}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# label demo
f_left = TkFrame.new(base_frame)
f_right = TkFrame.new(base_frame)
[f_left, f_right].each{|w| w.pack('side'=>'left', 'expand'=>'yes',
                                  'padx'=>10, 'pady'=>10, 'fill'=>'both')}

# label
[ TkLabel.new(f_left, 'text'=>'First label'),
  TkLabel.new(f_left, 'text'=>'Second label, raised',
              'relief'=>'raised'),
  TkLabel.new(f_left, 'text'=>'Third label, sunken', 'relief'=>'sunken')
].each{|w| w.pack('side'=>'top', 'expand'=>'yes', 'pady'=>2, 'anchor'=>'w')}

# TkLabel.new(f_right) {
Tk::Label.new(f_right) {
  bitmap('@' + [$demo_dir,'..','images','face.xbm'].join(File::Separator))
  borderwidth 2
  relief 'sunken'
}.pack('side'=>'top')

TkLabel.new(f_right) { text 'Tcl/Tk Proprietor' }.pack('side'=>'top')

