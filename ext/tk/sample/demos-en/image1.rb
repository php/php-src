## image1.rb
#
# This demonstration script displays two image widgets.
#
# two image widgets demo (called by 'widget')
#

# toplevel widget
if defined?($image1_demo) && $image1_demo
  $image1_demo.destroy
  $image1_demo = nil
end

# demo toplevel widget
$image1_demo = TkToplevel.new {|w|
  title('Image Demonstration #1')
  iconname("Image1")
  positionWindow(w)
}

base_frame = TkFrame.new($image1_demo).pack(:fill=>:both, :expand=>true)

# label
msg = TkLabel.new(base_frame) {
  font $font
  wraplength '4i'
  justify 'left'
  text "This demonstration displays two images, each in a separate label widget."
}
msg.pack('side'=>'top')

# frame
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    text 'Dismiss'
    command proc{
      tmppath = $image1_demo
      $image1_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'Show Code'
    command proc{showCode 'image1'}
  }.pack('side'=>'left', 'expand'=>'yes')

}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# image
image1a = \
TkPhotoImage.new('file'=>[$demo_dir,'..',
                          'images','earth.gif'].join(File::Separator))
image1b = \
TkPhotoImage.new('file'=>[$demo_dir,'..',
                          'images','earthris.gif'].join(File::Separator))

# label
#[ TkLabel.new(base_frame, 'image'=>image1a, 'bd'=>1, 'relief'=>'sunken'),
#  TkLabel.new(base_frame, 'image'=>image1b, 'bd'=>1, 'relief'=>'sunken')
#].each{|w| w.pack('side'=>'top', 'padx'=>'.5m', 'pady'=>'.5m')}
[ Tk::Label.new(base_frame, 'image'=>image1a, 'bd'=>1, 'relief'=>'sunken'),
  Tk::Label.new(base_frame, 'image'=>image1b, 'bd'=>1, 'relief'=>'sunken')
].each{|w| w.pack('side'=>'top', 'padx'=>'.5m', 'pady'=>'.5m')}

