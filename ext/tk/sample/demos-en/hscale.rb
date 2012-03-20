require "tkcanvas"

if defined?($hscale_demo) && $hscale_demo
  $hscale_demo.destroy
  $hscale_demo = nil
end

$hscale_demo = TkToplevel.new {|w|
  title("Horizontal Scale Demonstration")
  iconname("hscale")
}
positionWindow($hscale_demo)

base_frame = TkFrame.new($hscale_demo).pack(:fill=>:both, :expand=>true)

msg = TkLabel.new(base_frame) {
  font $font
  wraplength '3.5i'
  justify 'left'
  text "An arrow and a horizontal scale are displayed below.  If you click or drag mouse button 1 in the scale, you can change the length of the arrow."
}
msg.pack('side'=>'top')

TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    text 'Dismiss'
    command proc {
      tmppath = $hscale_demo
      $hscale_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'Show Code'
    command proc { showCode 'hscale' }
  }.pack('side'=>'left', 'expand'=>'yes')
}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')


def setWidth(w, width)
  width = width + 21
  x2 = width - 30
  if x2 < 21
    x2 = 21
  end
  w.coords 'poly',20,15,20,35,x2,35,x2,45,width,25,x2,5,x2,15,20,15
  w.coords 'line',20,15,20,35,x2,35,x2,45,width,25,x2,5,x2,15,20,15
end

TkFrame.new(base_frame) {|frame|
  canvas = TkCanvas.new(frame) {|c|
    width 50
    height 50
    bd 0
    highlightthickness 0
    TkcPolygon.new(c, '0', '0', '1', '1', '2', '2') {
      fill 'DeepSkyBlue'
      tags 'poly'
    }
    TkcLine.new(c, '0', '0', '1', '1', '2', '2', '0', '0') {
      fill 'black'
      tags 'line'
    }
  }.pack('side'=>'top', 'expand'=>'yes', 'anchor'=>'s', 'fill'=>'x', 'padx'=>'15')
  scale = TkScale.new(frame) {
    orient 'horizontal'
    length 284
    from 0
    to 250
    command proc{|value| setWidth(canvas, value)}
    tickinterval 50
  }.pack('side'=>'bottom', 'expand'=>'yes', 'anchor'=>'n')
  scale.set 75
}.pack('side'=>'top', 'fill'=>'x')
