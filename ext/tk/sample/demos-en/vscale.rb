# vscale.rb
#
# This demonstration script shows an example with a vertical scale.

require "tkcanvas"

if defined?($vscale_demo) && $vscale_demo
  $vscale_demo.destroy
  $vscale_demo = nil
end

$vscale_demo = TkToplevel.new {|w|
  title("Vertical Scale Demonstration")
  iconname("vscale")
}
positionWindow($vscale_demo)

base_frame = TkFrame.new($vscale_demo).pack(:fill=>:both, :expand=>true)

msg = TkLabel.new(base_frame) {
  font $font
  wraplength '3.5i'
  justify 'left'
  text "An arrow and a vertical scale are displayed below.  If you click or drag mouse button 1 in the scale, you can change the size of the arrow."
}
msg.pack('side'=>'top', 'padx'=>'.5c')

TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    text 'Dismiss'
    command proc {
      tmppath = $vscale_demo
      $vscale_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'Show Code'
    command proc { showCode 'vscale' }
  }.pack('side'=>'left', 'expand'=>'yes')
}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

def setHeight(w, height)
  height = height + 21
  y2 = height - 30
  if y2 < 21
    y2 = 21
  end
  w.coords 'poly',15,20,35,20,35,y2,45,y2,25,height,5,y2,15,y2,15,20
  w.coords 'line',15,20,35,20,35,y2,45,y2,25,height,5,y2,15,y2,15,20
end

TkFrame.new(base_frame) {|frame|
  borderwidth 10
  canvas = TkCanvas.new(frame) {|c|
    width 50
    height 50
    bd 0
    highlightthickness 0
    TkcPolygon.new(c, 0, 0, 1, 1, 2, 2) {
      fill 'SeaGreen3'
      tags 'poly'
    }
    TkcLine.new(c, 0, 0, 1, 1, 2, 2, 0, 0) {
      fill 'black'
      tags 'line'
    }
  }.pack('side'=>'left',  'anchor'=>'nw', 'fill'=>'y')
  scale = TkScale.new(frame) {
    orient 'vertical'
    length 284
    from 0
    to 250
    command proc{|value| setHeight(canvas, value)}
    tickinterval 50
  }.pack('side'=>'left', 'anchor'=>'ne')
  scale.set 75
}.pack
