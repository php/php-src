# image2.rb
#
# This demonstration script creates a simple collection of widgets
# that allow you to select and view images in a Tk label.
#
# widget demo 'load image' (called by 'widget')
#

# toplevel widget
if defined?($image2_demo) && $image2_demo
  $image2_demo.destroy
  $image2_demo = nil
end

# demo toplevel widget
$image2_demo = TkToplevel.new {|w|
  title('Image Demonstration #2')
  iconname("Image2")
  positionWindow(w)
}

base_frame = TkFrame.new($image2_demo).pack(:fill=>:both, :expand=>true)

# label
msg = TkLabel.new(base_frame) {
  font $font
  wraplength '4i'
  justify 'left'
  text "This demonstration allows you to view images using a Tk \"photo\" image.  First type a directory name in the listbox, then press Enter to load the directory into the listbox.  Then double-click on a file name in the listbox to see that image."
}
msg.pack('side'=>'top')

# frame
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    text 'Dismiss'
    command proc{
      tmppath = $image2_demo
      $image2_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'Show Code'
    command proc{showCode 'image2'}
  }.pack('side'=>'left', 'expand'=>'yes')

}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# create variable
$dirName = TkVariable.new([$demo_dir,'..','images'].join(File::Separator))

# image
$image2a = TkPhotoImage.new

#
TkLabel.new(base_frame, 'text'=>'Directory:')\
.pack('side'=>'top', 'anchor'=>'w')

image2_e = TkEntry.new(base_frame) {
  width 30
  textvariable $dirName
}.pack('side'=>'top', 'anchor'=>'w')

TkFrame.new(base_frame, 'height'=>'3m', 'width'=>20)\
.pack('side'=>'top', 'anchor'=>'w')

TkLabel.new(base_frame, 'text'=>'File:')\
.pack('side'=>'top', 'anchor'=>'w')

TkFrame.new(base_frame){|w|
  s = TkScrollbar.new(w)
  l = TkListbox.new(w) {
    width 20
    height 10
    yscrollcommand proc{|first,last| s.set first,last}
  }
  s.command(proc{|*args| l.yview(*args)})
  l.pack('side'=>'left', 'expand'=>'yes', 'fill'=>'y')
  s.pack('side'=>'left', 'expand'=>'yes', 'fill'=>'y')
  #l.insert(0,'earth.gif', 'earthris.gif', 'mickey.gif', 'teapot.ppm')
  l.insert(0,'earth.gif', 'earthris.gif', 'teapot.ppm')
  l.bind('Double-1', proc{|x,y| loadImage $image2a,l,x,y}, '%x %y')

  image2_e.bind 'Return', proc{loadDir l}

}.pack('side'=>'top', 'anchor'=>'w')

# image
[ TkFrame.new(base_frame, 'height'=>'3m', 'width'=>20),
  TkLabel.new(base_frame, 'text'=>'Image:'),
  TkLabel.new(base_frame, 'image'=>$image2a)
].each{|w| w.pack('side'=>'top', 'anchor'=>'w')}

#
def loadDir(w)
  w.delete(0,'end')
  Dir.glob([$dirName,'*'].join(File::Separator)).sort.each{|f|
    w.insert('end',File.basename(f))
  }
end

def loadImage(img,w,x,y)
  img.file([$dirName, w.get("@#{x},#{y}")].join(File::Separator))
end

