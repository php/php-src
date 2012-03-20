# -*- coding: utf-8 -*-
#
# animated wave demo (called by 'widget')
#
# based on Tcl/Tk8.5a2 widget demos

# destroy toplevel widget for this demo script
if defined?($aniwave_demo) && $aniwave_demo
  $aniwave_demo.destroy
  $aniwave_demo = nil
end

# create toplevel widget
$aniwave_demo = TkToplevel.new {|w|
  title("Animated Wave Demonstration")
  iconname("aniwave")
  positionWindow(w)
}

base_frame = TkFrame.new($aniwave_demo).pack(:fill=>:both, :expand=>true)

# create label
msg = TkLabel.new(base_frame) {
  font $font
  wraplength '4i'
  justify 'left'
  text 'このデモでは、ラインアイテムが一つだけ描かれたキャンバスウィジェットが表示されています。アニメーション処理は、そのラインアイテムの座標値を変更することで実現しています。'
}
msg.pack('side'=>'top')

# create frame
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    #text '了解'
    text '閉じる'
    command proc{
      tmppath = $aniwave_demo
      $aniwave_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'コード参照'
    command proc{showCode 'aniwave'}
  }.pack('side'=>'left', 'expand'=>'yes')

}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# animated wave
class AnimatedWaveDemo
  def initialize(frame, dir=:left)
    @direction = dir

    # create canvas widget
    @c = TkCanvas.new(frame, :width=>300, :height=>200,
                      :background=>'black')
    @c.pack(:padx=>10, :pady=>10, :expand=>true)

    # Creates a coordinates list of a wave.
    @waveCoords = []
    @backupCoords = []
    n = 0
    (-10..300).step(5){|n| @waveCoords << [n, 100]; @backupCoords << [n, 100] }
    n = 305
    @waveCoords << [n, 0]; @backupCoords << [n, 0]
    @waveCoords << [n+5, 200]; @backupCoords << [n+5, 200]
    @coordsLen = @waveCoords.length

    # Create a smoothed line and arrange for its coordinates to be the
    # contents of the variable waveCoords.
    @line = TkcLine.new(@c, @waveCoords,
                        :width=>1, :fill=>'green', :smooth=>true)

    # Main animation "loop".
    # Theoretically 100 frames-per-second (==10ms between frames)
    @timer = TkTimer.new(10){ basicMotion; reverser }

    # Arrange for the animation loop to stop when the canvas is deleted
    @c.bindtags_unshift(TkBindTag.new('Destroy'){ @timer.stop })
  end

  # Basic motion handler. Given what direction the wave is travelling
  # in, it advances the y coordinates in the coordinate-list one step in
  # that direction.
  def basicMotion
    @backupCoords, @waveCoords = @waveCoords, @backupCoords
    (0...@coordsLen).each{|idx|
      if @direction == :left
        @waveCoords[idx][1] = @backupCoords[(idx+1 == @coordsLen)? 0: idx+1][1]
      else
        @waveCoords[idx][1] = @backupCoords[(idx == 0)? -1: idx-1][1]
      end
    }
    @line.coords(@waveCoords)
  end

  # Oscillation handler. This detects whether to reverse the direction
  # of the wave by checking to see if the peak of the wave has moved off
  # the screen (whose size we know already.)
  def reverser
    if @waveCoords[0][1] < 10
      @direction = :right
    elsif @waveCoords[-1][1] < 10
      @direction = :left
    end
  end

  # animation control
  def move
    @timer.start
  end

  def stop
    @timer.stop
  end
end

# Start the animation processing
AnimatedWaveDemo.new(base_frame, :left).move
