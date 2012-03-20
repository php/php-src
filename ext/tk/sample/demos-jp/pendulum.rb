# -*- coding: utf-8 -*-
#
# This demonstration illustrates how Tcl/Tk can be used to construct
# simulations of physical systems.
# (called by 'widget')
#
# based on Tcl/Tk8.5a2 widget demos

# destroy toplevel widget for this demo script
if defined?($pendulum_demo) && $pendulum_demo
  $pendulum_demo.destroy
  $pendulum_demo = nil
end

# create toplevel widget
$pendulum_demo = TkToplevel.new {|w|
  title("Pendulum Animation Demonstration")
  iconname("pendulum")
  positionWindow(w)
}

base_frame = TkFrame.new($pendulum_demo).pack(:fill=>:both, :expand=>true)

# create label
msg = TkLabel.new(base_frame) {
  font $font
  wraplength '4i'
  justify 'left'
  text 'このデモは、物理系のシミュレーションに関わるようなアニメーション実行するために Ruby/Tk をどのように用いることができるかを示しています。左側のキャンバスは単純な振り子である物理系自体のグラフィカル表現であるのに対し、右側のキャンバスは系の位相空間のグラフ（角速度と角度とをプロットしたもの）になっています。左側のキャンバス上でクリックおよびドラッグを行って振り子の重りの位置を変えてみてください。'
}
msg.pack('side'=>'top')

# create frame
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    #text '了解'
    text '閉じる'
    command proc{
      tmppath = $pendulum_demo
      $pendulum_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'コード参照'
    command proc{showCode 'pendulum'}
  }.pack('side'=>'left', 'expand'=>'yes')

}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# animated wave
class PendulumAnimationDemo
  def initialize(frame)
    # Create some structural widgets
    @pane = TkPanedWindow.new(frame, :orient=>:horizontal).pack(:fill=>:both, :expand=>true)
#    @pane.add(@lf1 = TkLabelFrame.new(@pane, :text=>'Pendulum Simulation'))
#    @pane.add(@lf2 = TkLabelFrame.new(@pane, :text=>'Phase Space'))
    @lf1 = TkLabelFrame.new(@pane, :text=>'Pendulum Simulation')
    @lf2 = TkLabelFrame.new(@pane, :text=>'Phase Space')

    # Create the canvas containing the graphical representation of the
    # simulated system.
    @c = TkCanvas.new(@lf1, :width=>320, :height=>200, :background=>'white',
                      :borderwidth=>2, :relief=>:sunken)
    TkcText.new(@c, 5, 5, :anchor=>:nw,
                :text=>'Click to Adjust Bob Start Position')
    # Coordinates of these items don't matter; they will be set properly below
    @plate = TkcLine.new(@c, 0, 25, 320, 25, :width=>2, :fill=>'grey50')
    @rod = TkcLine.new(@c, 1, 1, 1, 1, :width=>3, :fill=>'black')
    @bob = TkcOval.new(@c, 1, 1, 2, 2,
                       :width=>3, :fill=>'yellow', :outline=>'black')
    TkcOval.new(@c, 155, 20, 165, 30, :fill=>'grey50', :outline=>'')

    # pack
    @c.pack(:fill=>:both, :expand=>true)

    # Create the canvas containing the phase space graph; this consists of
    # a line that gets gradually paler as it ages, which is an extremely
    # effective visual trick.
    @k = TkCanvas.new(@lf2, :width=>320, :height=>200, :background=>'white',
                      :borderwidth=>2, :relief=>:sunken)
    @y_axis = TkcLine.new(@k, 160, 200, 160, 0, :fill=>'grey75', :arrow=>:last)
    @x_axis = TkcLine.new(@k, 0, 100, 320, 100, :fill=>'grey75', :arrow=>:last)

    @graph = {}
    90.step(0, -10){|i|
      # Coordinates of these items don't matter;
      # they will be set properly below
      @graph[i] = TkcLine.new(@k, 0, 0, 1, 1, :smooth=>true, :fill=>"grey#{i}")
    }

    # labels
    @label_theta = TkcText.new(@k, 0, 0, :anchor=>:ne,
                               :text=>'q', :font=>'Symbol 8')
    @label_dtheta = TkcText.new(@k, 0, 0, :anchor=>:ne,
                               :text=>'dq', :font=>'Symbol 8')

    # pack
    @k.pack(:fill=>:both, :expand=>true)

    # Initialize some variables
    @points = []
    @theta = 45.0
    @dTheta = 0.0
    @length = 150

    # animation loop
    @timer = TkTimer.new(15){ repeat }

    # binding
    @c.bindtags_unshift(btag = TkBindTag.new)
    btag.bind('Destroy'){ @timer.stop }
    btag.bind('1', proc{|x, y| @timer.stop; showPendulum(x.to_i, y.to_i)},
              '%x %y')
    btag.bind('B1-Motion', proc{|x, y| showPendulum(x.to_i, y.to_i)}, '%x %y')
    btag.bind('ButtonRelease-1',
              proc{|x, y| showPendulum(x.to_i, y.to_i); @timer.start },
              '%x %y')

    btag.bind('Configure', proc{|w| @plate.coords(0, 25, w.to_i, 25)}, '%w')

    @k.bind('Configure', proc{|h, w|
              h = h.to_i
              w = w.to_i
              @psh = h/2;
              @psw = w/2
              @x_axis.coords(2, @psh, w-2, @psh)
              @y_axis.coords(@psw, h-2, @psw, 2)
              @label_theta.coords(@psw-4, 6)
              @label_dtheta.coords(w-6, @psh+4)
            }, '%h %w')

    # add
    Tk.update
    @pane.add(@lf1)
    @pane.add(@lf2)

    # init display
    showPendulum

    # animation start
    @timer.start(500)
  end

  # This procedure makes the pendulum appear at the correct place on the
  # canvas. If the additional arguments x, y are passed instead of computing
  # the position of the pendulum from the length of the pendulum rod and its
  # angle, the length and angle are computed in reverse from the given
  # location (which is taken to be the centre of the pendulum bob.)
  def showPendulum(x=nil, y=nil)
    if x && y && (x != 160 || y != 25)
      @dTheta = 0.0
      x2 = x - 160
      y2 = y - 25
      @length = Math.hypot(x2, y2)
      @theta = Math.atan2(x2,y2)*180/Math::PI
    else
      angle = @theta*Math::PI/180
      x = 160 + @length*Math.sin(angle)
      y = 25 + @length*Math.cos(angle)
    end

    @rod.coords(160, 25, x, y)
    @bob.coords(x-15, y-15, x+15, y+15)
  end

  # Update the phase-space graph according to the current angle and the
  # rate at which the angle is changing (the first derivative with
  # respect to time.)
  def showPhase
    unless @psw && @psh
      @psw = @k.width/2
      @psh = @k.height/2
    end
    @points << @theta + @psw << -20*@dTheta + @psh
    if @points.length > 100
      @points = @points[-100..-1]
    end
    (0...100).step(10){|i|
      first = - i
      last = 11 - i
      last = -1 if last >= 0
      next if first > last
      lst = @points[first..last]
      @graph[i].coords(lst) if lst && lst.length >= 4
    }
  end

  # This procedure is the "business" part of the simulation that does
  # simple numerical integration of the formula for a simple rotational
  # pendulum.
  def recomputeAngle
    scaling = 3000.0/@length/@length

    # To estimate the integration accurately, we really need to
    # compute the end-point of our time-step.  But to do *that*, we
    # need to estimate the integration accurately!  So we try this
    # technique, which is inaccurate, but better than doing it in a
    # single step.  What we really want is bound up in the
    # differential equation:
    #       ..             - sin theta
    #      theta + theta = -----------
    #                         length
    # But my math skills are not good enough to solve this!

    # first estimate
    firstDDTheta = -Math.sin(@theta * Math::PI/180) * scaling
    midDTheta = @dTheta + firstDDTheta
    midTheta = @theta + (@dTheta + midDTheta)/2
    # second estimate
    midDDTheta = -Math.sin(midTheta * Math::PI/180) * scaling
    midDTheta = @dTheta + (firstDDTheta + midDDTheta)/2
    midTheta = @theta + (@dTheta + midDTheta)/2
    # Now we do a double-estimate approach for getting the final value
    # first estimate
    midDDTheta = -Math.sin(midTheta * Math::PI/180) * scaling
    lastDTheta = midDTheta + midDDTheta
    lastTheta = midTheta + (midDTheta+ lastDTheta)/2
    # second estimate
    lastDDTheta = -Math.sin(lastTheta * Math::PI/180) * scaling
    lastDTheta = midDTheta + (midDDTheta + lastDDTheta)/2
    lastTheta = midTheta + (midDTheta + lastDTheta)/2
    # Now put the values back in our globals
    @dTheta = lastDTheta
    @theta = lastTheta
  end

  # This method ties together the simulation engine and the graphical
  # display code that visualizes it.
  def repeat
    # Simulate
    recomputeAngle

    # Update the display
    showPendulum
    showPhase
  end
end

# Start the animation processing
PendulumAnimationDemo.new(base_frame)
