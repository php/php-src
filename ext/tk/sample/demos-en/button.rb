# button.rb
#
# This demonstration script creates a toplevel window containing
# several button widgets.
#
# button widget demo (called by 'widget')
#

# toplevel widget
if defined?($button_demo) && $button_demo
  $button_demo.destroy
  $button_demo = nil
end

# demo toplevel widget
$button_demo = TkToplevel.new {|w|
  title("Button Demonstration")
  iconname("button")
  positionWindow(w)
}

# label
msg = TkLabel.new($button_demo) {
  font $kanji_font
  wraplength '4i'
  justify 'left'
  text "If you click on any of the four buttons below, the background of the button area will change to the color indicated in the button.  You can press Tab to move among the buttons, then press Space to invoke the current button."
}
msg.pack('side'=>'top')

# frame
$button_buttons = Tk::Frame.new($button_demo) {|frame|
  TkButton.new(frame) {
    text 'Dismiss'
    command proc{
      tmppath = $button_demo
      $button_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'See Code'
    command proc{showCode 'button'}
  }.pack('side'=>'left', 'expand'=>'yes')

}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# button
TkButton.new($button_demo){
  text "Peach Puff"
  width 10
  command proc{
    $button_demo.configure('bg','PeachPuff1')
    $button_buttons.configure('bg','PeachPuff1')
  }
}.pack('side'=>'top', 'expand'=>'yes', 'pady'=>2)

TkButton.new($button_demo){
  text "Light Blue"
  width 10
  command proc{
    $button_demo.configure('bg','LightBlue1')
    $button_buttons.configure('bg','LightBlue1')
  }
}.pack('side'=>'top', 'expand'=>'yes', 'pady'=>2)

TkButton.new($button_demo){
  text "Sea Green"
  width 10
  command proc{
    $button_demo.configure('bg','SeaGreen2')
    $button_buttons.configure('bg','SeaGreen2')
  }
}.pack('side'=>'top', 'expand'=>'yes', 'pady'=>2)

TkButton.new($button_demo){
  text "Yellow"
  width 10
  command proc{
    $button_demo.configure('bg','Yellow1')
    $button_buttons.configure('bg','Yellow1')
  }
}.pack('side'=>'top', 'expand'=>'yes', 'pady'=>2)
