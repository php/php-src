# icon.rb
#
# This demonstration script creates a toplevel window containing
# buttons that display bitmaps instead of text.
#
# iconic button widget demo (called by 'widget')
#

# toplevel widget
if defined?($icon_demo) && $icon_demo
  $icon_demo.destroy
  $icon_demo = nil
end

# demo toplevel widget
$icon_demo = TkToplevel.new {|w|
  title("Iconic Button Demonstration")
  iconname("icon")
  positionWindow(w)
}

base_frame = TkFrame.new($icon_demo).pack(:fill=>:both, :expand=>true)

# label
msg = TkLabel.new(base_frame) {
  font $font
  wraplength '5i'
  justify 'left'
  text "This window shows three ways of using bitmaps or images in radiobuttons and checkbuttons.  On the left are two radiobuttons, each of which displays a bitmap and an indicator.  In the middle is a checkbutton that displays a different image depending on whether it is selected or not.  On the right is a checkbutton that displays a single bitmap but changes its background color to indicate whether or not it is selected. (This change is visible when the mouse pointer is not directy over the button.)"
}
msg.pack('side'=>'top')

# frame
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    text 'Dismiss'
    command proc{
      tmppath = $icon_demo
      $icon_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'Show Code'
    command proc{showCode 'icon'}
  }.pack('side'=>'left', 'expand'=>'yes')

}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# image
flagup = \
TkBitmapImage.new('file'=>[$demo_dir,'..',
                           'images','flagup.xbm'].join(File::Separator),
                  'maskfile'=>\
                  [$demo_dir,'..','images','flagup.xbm'].join(File::Separator))
flagdown = \
TkBitmapImage.new('file'=>[$demo_dir,'..',
                           'images','flagdown.xbm'].join(File::Separator),
                  'maskfile'=>\
                  [$demo_dir,'..',
                    'images','flagdown.xbm'].join(File::Separator))

# create variable
letters = TkVariable.new

# frame
TkFrame.new(base_frame, 'borderwidth'=>10){|w|
  TkFrame.new(w) {|f|
    # TkRadioButton.new(f){
    Tk::RadioButton.new(f){
      bitmap '@' + [$demo_dir,'..',
                    'images','letters.xbm'].join(File::Separator)
      variable letters
      value 'full'
    }.pack('side'=>'top', 'expand'=>'yes')

    # TkRadioButton.new(f){
    Tk::RadioButton.new(f){
      bitmap '@' + [$demo_dir,'..',
                     'images','noletter.xbm'].join(File::Separator)
      variable letters
      value 'empty'
    }.pack('side'=>'top', 'expand'=>'yes')

  }.pack('side'=>'left', 'expand'=>'yes', 'padx'=>'5m')

  # TkCheckButton.new(w) {
  Tk::CheckButton.new(w) {
    image flagdown
    selectimage flagup
    indicatoron 0
    selectcolor self['background']
  }.pack('side'=>'left', 'expand'=>'yes', 'padx'=>'5m')

  # TkCheckButton.new(w) {
  Tk::CheckButton.new(w) {
    bitmap '@' + [$demo_dir,'..',
                   'images','letters.xbm'].join(File::Separator)
    indicatoron 0
    selectcolor 'SeaGreen1'
  }.pack('side'=>'left', 'expand'=>'yes', 'padx'=>'5m')

}.pack('side'=>'top')

