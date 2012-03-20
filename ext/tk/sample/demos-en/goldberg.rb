#
# Ruby/Tk Goldverg demo (called by 'widget')
#
# Based on Tcl/Tk8.5a2 widget demos.
# The following is the original comment of TkGoldberg.tcl.
#
#>>##+#################################################################
#>>#
#>># TkGoldberg.tcl
#>># by Keith Vetter, March 13, 2003
#>>#
#>># "Man will always find a difficult means to perform a simple task"
#>># Rube Goldberg
#>>#
#>># Reproduced here with permission.
#>>#
#>>##+#################################################################
#>>#
#>># Keith Vetter 2003-03-21: this started out as a simple little program
#>># but was so much fun that it grew and grew. So I apologize about the
#>># size but I just couldn't resist sharing it.
#>>#
#>># This is a whizzlet that does a Rube Goldberg type animation, the
#>># design of which comes from an New Years e-card from IncrediMail.
#>># That version had nice sound effects which I eschewed. On the other
#>># hand, that version was in black and white (actually dark blue and
#>># light blue) and this one is fully colorized.
#>>#
#>># One thing I learned from this project is that drawing filled complex
#>># objects on a canvas is really hard. More often than not I had to
#>># draw each item twice--once with the desired fill color but no
#>># outline, and once with no fill but with the outline. Another trick
#>># is erasing by drawing with the background color. Having a flood fill
#>># command would have been extremely helpful.
#>>#
#>># Two wiki pages were extremely helpful: Drawing rounded rectangles
#>># which I generalized into Drawing rounded polygons, and regular
#>># polygons which allowed me to convert ovals and arcs into polygons
#>># which could then be rotated (see Canvas Rotation). I also wrote
#>># Named Colors to aid in the color selection.
#>>#
#>># I could comment on the code, but it's just 26 state machines with
#>># lots of canvas create and move calls.

if defined?($goldberg_demo) && $goldberg_demo
  $goldberg_demo.destroy
  $goldberg_demo = nil
end

# demo toplevel widget
$goldberg_demo = TkToplevel.new {|w|
  title("Tk Goldberg (demonstration)")
  iconname("goldberg")
#  positionWindow(w)
}

base_frame = TkFrame.new($goldberg_demo).pack(:fill=>:both, :expand=>true)

=begin
# label
msg = TkLabel.new($goldberg_demo) {
  font 'Arial 10'
  wraplength '4i'
  justify 'left'
  text "This is a demonstration of just how complex you can make your animations become. Click the ball to start things moving!\n\n\"Man will always find a difficult means to perform a simple task\"\n - Rube Goldberg"
}
msg.pack('side'=>'top')
=end

=begin
# frame
TkFrame.new($goldberg_demo) {|frame|
  TkButton.new(frame) {
    text 'Dismiss'
    command proc{
      tmppath = $goldberg_demo
      $goldberg_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'See Code'
    command proc{showCode 'goldberg'}
  }.pack('side'=>'left', 'expand'=>'yes')

}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')
=end

#########################################

class TkGoldberg_Demo
  def initialize(parent)
    @parent = parent

    @S = {}
    @S['title']   = 'Tk Goldberg'
    @S['speed']   = TkVariable.new(5)
    @S['cnt']     = TkVariable.new(0)
    @S['message'] = TkVariable.new("\\nWelcome\\nto\\nRuby/Tk")
    @S['pause']   = TkVariable.new
    @S['details'] = TkVariable.new(true)

    @S['mode'] = TkVariable.new(:MSTART, :symbol)
    #            :MSTART, :MGO, :MPAUSE, :MSSTEP, :MBSTEP, :MDONE, :MDEBUG

    #         0,  1,  2,  3,  4,  5,   6,   7,   8,   9,  10
    @speed = [1, 10, 20, 50, 80, 100, 150, 200, 300, 400, 500]

    # colors
    @C = {}
    @C['fg'] = 'black'
    # @C['bg'] = 'gray75'
    @C['bg'] = 'cornflowerblue'

    @C['0'] = 'white';         @C['1a'] = 'darkgreen';   @C['1b'] = 'yellow'
    @C['2'] = 'red';           @C['3a'] = 'green';       @C['3b'] = 'darkblue'
    @C['4'] = @C['fg'];        @C['5a'] = 'brown';       @C['5b'] = 'white'
    @C['6'] = 'magenta';       @C['7'] = 'green';        @C['8'] = @C['fg']
    @C['9'] = 'blue4';         @C['10a'] = 'white';      @C['10b'] = 'cyan'
    @C['11a'] = 'yellow';      @C['11b'] = 'mediumblue'; @C['12'] = 'tan2'
    @C['13a'] = 'yellow';      @C['13b'] = 'red';        @C['14'] = 'white'
    @C['15a'] = 'green';       @C['15b'] = 'yellow';     @C['16'] = 'gray65'
    @C['17'] = '#A65353';     @C['18'] = @C['fg'];      @C['19'] = 'gray50'
    @C['20'] = 'cyan';         @C['21'] = 'gray65';      @C['22'] = @C['20']
    @C['23a'] = 'blue';        @C['23b'] = 'red';        @C['23c'] = 'yellow'
    @C['24a'] = 'red';         @C['24b'] = 'white';

    @STEP = TkVariable.new_hash
    @STEP.default_value_type = :numeric

    @XY = {}

    @XY6 = {
      '-1'=>[366, 207], '-2'=>[349, 204], '-3'=>[359, 193], '-4'=>[375, 192],
      '-5'=>[340, 190], '-6'=>[349, 177], '-7'=>[366, 177], '-8'=>[380, 176],
      '-9'=>[332, 172], '-10'=>[342, 161], '-11'=>[357, 164],
      '-12'=>[372, 163], '-13'=>[381, 149], '-14'=>[364, 151],
      '-15'=>[349, 146], '-16'=>[333, 148], '0'=>[357, 219],
      '1'=>[359, 261], '2'=>[359, 291], '3'=>[359, 318], '4'=>[361, 324],
      '5'=>[365, 329], '6'=>[367, 334], '7'=>[367, 340], '8'=>[366, 346],
      '9'=>[364, 350], '10'=>[361, 355], '11'=>[359, 370], '12'=>[359, 391],
      '13,0'=>[360, 456], '13,1'=>[376, 456], '13,2'=>[346, 456],
      '13,3'=>[330, 456], '13,4'=>[353, 444], '13,5'=>[368, 443],
      '13,6'=>[339, 442], '13,7'=>[359, 431], '13,8'=>[380, 437],
      '13,9'=>[345, 428], '13,10'=>[328, 434], '13,11'=>[373, 424],
      '13,12'=>[331, 420], '13,13'=>[360, 417], '13,14'=>[345, 412],
      '13,15'=>[376, 410], '13,16'=>[360, 403]
    }

    @timer = TkTimer.new(@speed[@S['speed'].numeric]){|timer|
      timer.set_interval(go)
    }

    do_display
    reset

    # Start everything going
    @timer.start
  end

  def do_display()
    @ctrl = TkFrame.new(@parent, :relief=>:ridge, :bd=>2, :padx=>5, :pady=>5)
    @screen = TkFrame.new(@parent, :bd=>2,
                         :relief=>:raised).pack(:side=>:left, :fill=>:both,
                                                :expand=>true)

    @canvas = TkCanvas.new(@parent, :width=>850, :height=>700,
                          :bg=>@C['bg'], :highlightthickness=>0){
      scrollregion([0, 0, 1000, 1000]) # Kludge to move everything up
      yview_moveto(0.05)
    }.pack(:in=>@screen, :side=>:top, :fill=>:both, :expand=>true)

    @canvas.bind('3'){ @pause.invoke }
    @canvas.bind('Destroy'){ @timer.stop }

    do_ctrl_frame
    do_detail_frame

    # msg = TkLabel.new(@parent, :bg=>@C['bg'], :fg=>'white') {
    msg = Tk::Label.new(@parent, :bg=>@C['bg'], :fg=>'white') {
      font 'Arial 10'
      wraplength 600
      justify 'left'
      text "This is a demonstration of just how complex you can make your animations become. Click the ball to start things moving!\n\"Man will always find a difficult means to perform a simple task\" - Rube Goldberg"
    }
    msg.place(:in=>@canvas, :relx=>0, :rely=>0, :anchor=>:nw)

    frame = TkFrame.new(@parent, :bg=>@C['bg'])

    # TkButton.new(frame, :bg=>@C['bg'], :activebackground=>@C['bg']) {
    Tk::Button.new(frame, :bg=>@C['bg'], :activebackground=>@C['bg']) {
      text 'Dismiss'
      command proc{
        tmppath = $goldberg_demo
        $goldberg_demo = nil
        tmppath.destroy
      }
    }.pack('side'=>'left')

    # TkButton.new(frame, :bg=>@C['bg'], :activebackground=>@C['bg']) {
    Tk::Button.new(frame, :bg=>@C['bg'], :activebackground=>@C['bg']) {
      text 'See Code'
      command proc{showCode 'goldberg'}
    }.pack('side'=>'left', 'padx'=>5)

    # @show = TkButton.new(frame, :text=>'>>', :command=>proc{show_ctrl},
    @show = Tk::Button.new(frame, :text=>'>>', :command=>proc{show_ctrl},
                         :bg=>@C['bg'], :activebackground=>@C['bg'])
    @show.pack('side'=>'left')
    frame.place(:in=>@canvas, :relx=>1, :rely=>0, :anchor=>:ne)

    Tk.update
  end

  def do_ctrl_frame
    @start = Tk::Button.new(@parent, :text=>'Start', :bd=>6,
                          :command=>proc{do_button(0)})
    if font = @start['font']
      @start.font(font.weight('bold'))
    end

    @pause = TkCheckbutton.new(@parent, :text=>'Pause', :font=>font,
                               :command=>proc{do_button(1)}, :relief=>:raised,
                               :variable=>@S['pause'])

    @step  = TkButton.new(@parent, :text=>'Single Step', :font=>font,
                          :command=>proc{do_button(2)})
    @bstep = TkButton.new(@parent, :text=>'Big Step', :font=>font,
                          :command=>proc{do_button(4)})
    @reset = TkButton.new(@parent, :text=>'Reset', :font=>font,
                          :command=>proc{do_button(3)})

    @details = TkFrame.new(@parent, :bd=>2, :relief=>:ridge)
    @detail = TkCheckbutton.new(@parent, :text=>'Details', :font=>font,
                               :relief=>:raised, :variable=>@S['details'])

    @msg_entry = TkEntry.new(@parent, :textvariable=>@S['message'],
                             :justify=>:center)
    @speed_scale = TkScale.new(@parent, :orient=>:horizontal,
                               :from=>1, :to=>10, :font=>font,
                               :variable=>@S['speed'], :bd=>2,
                               :relief=>:ridge, :showvalue=>false)
    @about = TkButton.new(@parent, :text=>'About',
                          :command=>proc{about}, :font=>font)

    Tk.grid(@start, :in=>@ctrl, :row=>0, :sticky=>:ew)
    @ctrl.grid_rowconfigure(1, :minsize=>10)
    Tk.grid(@pause, :in=>@ctrl, :row=>2, :sticky=>:ew)
    Tk.grid(@step,  :in=>@ctrl, :sticky=>:ew)
    Tk.grid(@bstep, :in=>@ctrl, :sticky=>:ew)
    Tk.grid(@reset, :in=>@ctrl, :sticky=>:ew)
    @ctrl.grid_rowconfigure(10, :minsize=>20)
    Tk.grid(@details, :in=>@ctrl, :row=>11, :sticky=>:ew)
    Tk.grid(@detail, :in=>@details, :row=>0, :sticky=>:ew)
    @ctrl.grid_rowconfigure(50, :weight=>1)

    @S['mode'].trace('w', proc{|*args| active_GUI(*args)})
    @S['details'].trace('w', proc{|*args| active_GUI(*args)})
    @S['speed'].trace('w', proc{|*args| active_GUI(*args)})

    Tk.grid(@msg_entry, :in=>@ctrl, :row=>98, :sticky=>:ew, :pady=>5)
    Tk.grid(@speed_scale, :in=>@ctrl, :row=>99, :sticky=>:ew)
    Tk.grid(@about, :in=>@ctrl, :row=>100, :sticky=>:ew)

    @reset.bind('3'){@S['mode'].value = -1}  # Debugging
  end

  def do_detail_frame
    @f_details = TkFrame.new(@details)

    @label = TkLabel.new(@f_details, :textvariable=>@S['cnt'],
                         :bd=>1, :relief=>:solid, :bg=>'white')
    Tk.grid(@label, '-', '-', '-', :sticky=>:ew, :row=>0)

    idx = 1
    loop {
      break unless respond_to?("move#{idx}")
      l = TkLabel.new(@f_details, :text=>idx, :anchor=>:e,
                      :width=>2, :bd=>1, :relief=>:solid, :bg=>'white')
      @STEP[idx] = 0
      ll = TkLabel.new(@f_details, :textvariable=>@STEP.ref(idx),
                       :width=>5, :bd=>1, :relief=>:solid, :bg=>'white')
      row = (idx + 1)/2
      col = ((idx + 1) & 1) * 2
      Tk.grid(l, :sticky=>:ew, :row=>row, :column=>col)
      Tk.grid(ll, :sticky=>:ew, :row=>row, :column=>(col + 1))
      idx += 1
    }
    @f_details.grid_columnconfigure(1, :weight=>1)
  end

  def show_ctrl
    if @ctrl.winfo_mapped?
      @ctrl.pack_forget
      @show.text('>>')
    else
      @ctrl.pack(:side=>:right, :fill=>:both, :ipady=>5)
      @show.text('<<')
    end
  end

  def draw_all
    reset_step
    @canvas.delete(:all)
    idx = 0
    loop{
      m = "draw#{idx}"
      break unless respond_to?(m)
      send(m)
      idx += 1
    }
  end

  def active_GUI(var1, var2, op)
    st = {false=>:disabled, true=>:normal}

    m = @S['mode'].to_sym
    @S['pause'].value = (m == :MPAUSE)
    @start.state(st[m != :MGO])
    @pause.state(st[m != :MSTART && m != :MDONE])
    @step.state(st[m != :MGO && m != :MDONE])
    @bstep.state(st[m != :MGO && m != :MDONE])
    @reset.state(st[m != :MSTART])

    if @S['details'].bool
      Tk.grid(@f_details, :in=>@details, :row=>2, :sticky=>:ew)
    else
      Tk.grid_forget(@f_details)
    end
    @speed_scale.label("Speed: #{@S['speed'].value}")
  end

  def start
    @S['mode'].value = :MGO
  end

  def do_button(what)
    case what
    when 0  # Start
      reset if @S['mode'].to_sym == :MDONE
      @S['mode'].value = :MGO

    when 1  # Pause
      @S['mode'].value = ((@S['pause'].bool)? :MPAUSE: :MGO)

    when 2  # Step
      @S['mode'].value = :MSSTEP

    when 3  # Reset
      reset

    when 4  # Big step
      @S['mode'].value = :MBSTEP
    end
  end

  def go(who = nil)
    now = Tk::Clock.clicks(:miliseconds)
    if who  # Start here for debugging
      @S['active'] = [who]
      @S['mode'].value = :MGO
    end
    return if @S['mode'].to_sym == :MDEBUG  # Debugging
    # If not paused, do the next move
    n = next_step if @S['mode'].to_sym != :MPAUSE
    @S['mode'].value = :MPAUSE if @S['mode'].to_sym == :MSSTEP  # Single step
    @S['mode'].value = :MSSTEP if @S['mode'].to_sym == :MBSTEP && n  # big step
    elapsed = Tk::Clock.clicks(:miliseconds) - now
    delay = @speed[@S['speed'].to_i] - elapsed
    delay = 1 if delay <= 0
    return delay
  end

  def next_step
    retval = false   # Return value

    if @S['mode'].to_sym != :MSTART && @S['mode'].to_sym != :MDONE
      @S['cnt'].numeric += 1
    end
    alive = []
    @S['active'].each{|who|
      who = who.to_i
      n = send("move#{who}")
      if (n & 1).nonzero?          # This guy still alive
        alive << who
      end
      if (n & 2).nonzero?          # Next guy is active
        alive << (who + 1)
        retval = true
      end
      if (n & 4).nonzero?          # End of puzzle flag
        @S['mode'].value = :MDONE  # Done mode
        @S['active'] = []          # No more animation
        return true
      end
    }
    @S['active'] = alive
    return retval
  end

  def about
    msg = "Ruby/Tk Version ::\nby Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)\n\n"
    msg += "Original Version ::\n"
    msg += "#{@S['title']}\nby Keith Vetter, March 2003\n(Reproduced by kind permission of the author)\n\n"
    msg += "Man will always find a difficult means to perform a simple task"
    msg += "\nRube Goldberg"
    Tk.messageBox(:message=>msg, :title=>'About')
  end

  ################################################################
  #
  # All the drawing and moving routines
  #

  # START HERE! banner
  def draw0
    color = @C['0']
    TkcText.new(@canvas, [579, 119], :text=>'START HERE!',
                :fill=>color, :anchor=>:w,
                :tag=>'I0', :font=>['Times Roman', 12, :italic, :bold])
    TkcLine.new(@canvas, [719, 119, 763, 119], :tag=>'I0', :fill=>color,
                :width=>5, :arrow=>:last, :arrowshape=>[18, 18, 5])
    @canvas.itembind('I0', '1'){ start }
  end

  def move0(step = nil)
    step = get_step(0, step)

    if @S['mode'].to_sym != :MSTART    # Start the ball rolling
      move_abs('I0', [-100, -100])     # Hide the banner
      return 2
    end

    pos = [
      [673, 119], [678, 119], [683, 119], [688, 119],
      [693, 119], [688, 119], [683, 119], [678, 119]
    ]
    step = step % pos.length
    move_abs('I0', pos[step])
    return 1
  end

  # Dropping ball
  def draw1
    color = @C['1a']
    color2 = @C['1b']
    TkcPolygon.new(@canvas,
                   [ 844, 133, 800, 133, 800, 346, 820, 346,
                     820, 168, 844, 168, 844, 133 ],
                   :width=>3, :fill=>color, :outline=>'')
    TkcPolygon.new(@canvas,
                   [ 771, 133, 685, 133, 685, 168, 751, 168,
                     751, 346, 771, 346, 771, 133 ],
                   :width=>3, :fill=>color, :outline=>'')
    TkcOval.new(@canvas, box(812, 122, 9),
                :tag=>'I1', :fill=>color2, :outline=>'')

    @canvas.itembind('I1', '1'){ start }
  end

  def move1(step = nil)
    step = get_step(1, step)
    pos = [
      [807, 122], [802, 122], [797, 123], [793, 124], [789, 129], [785, 153],
      [785, 203], [785, 278, :x], [785, 367], [810, 392], [816, 438],
      [821, 503], [824, 585, :y], [838, 587], [848, 593], [857, 601],
      [-100, -100]
    ]
    return 0 if step >= pos.length
    where = pos[step]
    move_abs('I1', where)
    move15a if where[2] == :y
    return 3 if where[2] == :x
    return 1
  end

  # Lighting the match
  def draw2
    color = @C['2']

    # Fulcrum
    TkcPolygon.new(@canvas, [750, 369, 740, 392, 760, 392],
                   :fill=>@C['fg'], :outline=>@C['fg'])

    # Strike box
    TkcRectangle.new(@canvas, [628, 335, 660, 383],
                     :fill=>'', :outline=>@C['fg'])
    (0..2).each{|y|
      yy = 335 + y*16
      TkcBitmap.new(@canvas, [628, yy], :bitmap=>'gray25',
                    :anchor=>:nw, :foreground=>@C['fg'])
      TkcBitmap.new(@canvas, [644, yy], :bitmap=>'gray25',
                    :anchor=>:nw, :foreground=>@C['fg'])
    }

    # Lever
    TkcLine.new(@canvas, [702, 366, 798, 366],
                :fill=>@C['fg'], :width=>6, :tag=>'I2_0')

    # R strap
    TkcLine.new(@canvas, [712, 363, 712, 355],
                :fill=>@C['fg'], :width=>3, :tag=>'I2_1')

    # L strap
    TkcLine.new(@canvas, [705, 363, 705, 355],
                :fill=>@C['fg'], :width=>3, :tag=>'I2_2')

    # Match stick
    TkcLine.new(@canvas, [679, 356, 679, 360, 717, 360, 717, 356, 679, 356],
                :fill=>@C['fg'], :width=>3, :tag=>'I2_3')

    # Match head
    TkcPolygon.new(@canvas,
                   [ 671, 352, 677.4, 353.9, 680, 358.5, 677.4, 363.1,
                     671, 365, 664.6, 363.1, 662, 358.5, 664.6, 353.9 ],
                   :fill=>color, :outline=>color, :tag=>'I2_4')
  end

  def move2(step = nil)
    step = get_step(2, step)

    stages = [0, 0, 1, 2, 0, 2, 1, 0, 1, 2, 0, 2, 1]
    xy = []
    xy[0] = [
      686, 333, 692, 323, 682, 316, 674, 309, 671, 295, 668, 307,
      662, 318, 662, 328, 671, 336
    ]
    xy[1] = [
      687, 331, 698, 322, 703, 295, 680, 320, 668, 297, 663, 311,
      661, 327, 671, 335
    ]
    xy[2] = [
      686, 331, 704, 322, 688, 300, 678, 283, 678, 283, 674, 298,
      666, 309, 660, 324, 672, 336
    ]

    if step >= stages.length
      @canvas.delete('I2')
      return 0
    end

    if step == 0  # Rotate the match
      beta = 20

      ox, oy = anchor('I2_0', :s)  # Where to pivot

      i = 0
      until @canvas.find_withtag("I2_#{i}").empty?
        rotate_item("I2_#{i}", ox, oy, beta)
        i += 1
      end

      # For the flame
      TkcPolygon.new(@canvas, [], :tag=>'I2', :smooth=>true, :fill=>@C['2'])

      return 1
    end
    @canvas.coords('I2', xy[stages[step]])
    return ((step == 7)? 3: 1)
  end

  # Weight and pulleys
  def draw3
    color = @C['3a']
    color2 = @C['3b']

    xy = [ [602, 296], [577, 174], [518, 174] ]
    xy.each{|x, y| # 3 Pulleys
      TkcOval.new(@canvas, box(x, y, 13),
                  :fill=>color, :outline=>@C['fg'], :width=>3)
      TkcOval.new(@canvas, box(x, y, 2), :fill=>@C['fg'], :outline=>@C['fg'])
    }

    # Wall to flame
    TkcLine.new(@canvas, [750, 309, 670, 309], :tag=>'I3_s',
                :width=>3, :fill=>@C['fg'], :smooth=>true)

    # Flame to pulley 1
    TkcLine.new(@canvas, [670, 309, 650, 309], :tag=>'I3_0',
                :width=>3, :fill=>@C['fg'], :smooth=>true)
    TkcLine.new(@canvas, [650, 309, 600, 309], :tag=>'I3_1',
                :width=>3, :fill=>@C['fg'], :smooth=>true)

    # Pulley 1 half way to 2
    TkcLine.new(@canvas, [589, 296, 589, 235], :tag=>'I3_2',
                :width=>3, :fill=>@C['fg'])

    # Pulley 1 other half to 2
    TkcLine.new(@canvas, [589, 235, 589, 174], :width=>3, :fill=>@C['fg'])

    # Across the top
    TkcLine.new(@canvas, [577, 161, 518, 161], :width=>3, :fill=>@C['fg'])

    # Down to weight
    TkcLine.new(@canvas, [505, 174, 505, 205], :tag=>'I3_w',
                :width=>3, :fill=>@C['fg'])

    # Draw the weight as 2 circles, two rectangles and 1 rounded rectangle
    x1, y1, x2, y2 = [515, 207, 495, 207]
    TkcOval.new(@canvas, box(x1, y1, 6),
                :tag=>'I3_', :fill=>color2, :outline=>color2)
    TkcOval.new(@canvas, box(x2, y2, 6),
                :tag=>'I3_', :fill=>color2, :outline=>color2)
    TkcRectangle.new(@canvas, x1, y1 - 6, x2, y2 + 6,
                     :tag=>'I3_', :fill=>color2, :outline=>color2)

    TkcPolygon.new(@canvas, round_rect([492, 220, 518, 263], 15),
                   :smooth=>true, :tag=>'I3_', :fill=>color2, :outline=>color2)

    TkcLine.new(@canvas, [500, 217, 511, 217],
                :tag=>'I3_', :fill=>color2, :width=>10)

    # Bottom weight target
    TkcLine.new(@canvas, [502, 393, 522, 393, 522, 465],
                :tag=>'I3__', :fill=>@C['fg'], :joinstyle=>:miter, :width=>10)
  end

  def move3(step = nil)
    step = get_step(3, step)

    pos = [ [505, 247], [505, 297], [505, 386.5], [505, 386.5] ]
    rope = []
    rope[0] = [750, 309, 729, 301, 711, 324, 690, 300]
    rope[1] = [750, 309, 737, 292, 736, 335, 717, 315, 712, 320]
    rope[2] = [750, 309, 737, 309, 740, 343, 736, 351, 725, 340]
    rope[3] = [750, 309, 738, 321, 746, 345, 742, 356]

    return 0 if step >= pos.length

    @canvas.delete("I3_#{step}")        # Delete part of the rope
    move_abs('I3_', pos[step])          # Move weight down
    @canvas.coords('I3_s', rope[step])  # Flapping rope end
    @canvas.coords('I3_w', [505, 174].concat(pos[step]))
    if step == 2
      @canvas.move('I3__', 0, 30)
      return 2
    end
    return 1
  end

  # Cage and door
  def draw4
    color = @C['4']
    x0, y0, x1, y1 = [527, 356, 611, 464]

    # Horizontal bars
    y0.step(y1, 12){|y|
      TkcLine.new(@canvas, [x0, y, x1, y], :fill=>color, :width=>1)
    }

    # Vertical bars
    x0.step(x1, 12){|x|
      TkcLine.new(@canvas, [x, y0, x, y1], :fill=>color, :width=>1)
    }

    # Swing gate
    TkcLine.new(@canvas, [518, 464, 518, 428],
                :tag=>'I4', :fill=>color, :width=>1)
  end

  def move4(step = nil)
    step = get_step(4, step)

    angles = [-10, -20, -30, -30]
    return 0 if step >= angles.length

    rotate_item('I4', 518, 464, angles[step])
    @canvas.raise('I4')

    return((step == 3)? 3: 1)
  end

  # Mouse
  def draw5
    color  = @C['5a']
    color2 = @C['5b']

    xy = [377, 248, 410, 248, 410, 465, 518, 465]  # Mouse course
    xy.concat [518, 428, 451, 428, 451, 212, 377, 212]

    TkcPolygon.new(@canvas, xy, :fill=>color2, :outline=>@C['fg'], :width=>3)

    xy = [
      534.5, 445.5, 541, 440, 552, 436, 560, 436, 569, 440, 574, 446,
      575, 452, 574, 454, 566, 456, 554, 456, 545, 456, 537, 454, 530, 452
    ]
    TkcPolygon.new(@canvas, xy, :tag=>['I5', 'I5_0'], :fill=>color)

    TkcLine.new(@canvas, [573, 452, 592, 458, 601, 460, 613, 456], # Tail
                :tag=>['I5', 'I5_1'], :fill=>color, :smooth=>true, :width=>3)

    xy = box(540, 446, 2)   # Eye
    xy = [540, 444, 541, 445, 541, 447, 540, 448, 538, 447, 538, 445]
    TkcPolygon.new(@canvas, xy, :tag=>['I5', 'I5_2'], :fill=>@C['bg'],
                   :outline=>'', :smooth=>true)

    xy = [538, 454, 535, 461] # Front leg
    TkcLine.new(@canvas, xy, :tag=>['I5', 'I5_3'], :fill=>color, :width=>2)

    xy = [566, 455, 569, 462] # Back leg
    TkcLine.new(@canvas, xy, :tag=>['I5', 'I5_4'], :fill=>color, :width=>2)

    xy = [544, 455, 545, 460] # 2nd front leg
    TkcLine.new(@canvas, xy, :tag=>['I5', 'I5_5'], :fill=>color, :width=>2)

    xy = [560, 455, 558, 460] # 2nd back leg
    TkcLine.new(@canvas, xy, :tag=>['I5', 'I5_6'], :fill=>color, :width=>2)
  end

  def move5(step = nil)
    step = get_step(5, step)

    pos = [
      [553, 452], [533, 452], [513, 452], [493, 452], [473, 452],
      [463, 442, 30], [445.5, 441.5, 30], [425.5, 434.5, 30], [422, 414],
      [422, 394], [422, 374], [422, 354], [422, 334], [422, 314], [422, 294],
      [422, 274, -30], [422, 260.5, -30, :x], [422.5, 248.5, -28], [425, 237]
    ]

    return 0 if step >= pos.length

    x, y, beta, nxt = pos[step]
    move_abs('I5', [x, y])
    if beta
      ox, oy = centroid('I5_0')
      (0..6).each{|id| rotate_item("I5_#{id}", ox, oy, beta) }
    end
    return 3 if nxt == :x
    return 1
  end

  # Dropping gumballs
  def draw6
    color = @C['6']
    xy = [324, 130, 391, 204] # Ball holder
    xy = round_rect(xy, 10)
    TkcPolygon.new(@canvas, xy, :smooth=>true,
                   :outline=>@C['fg'], :width=>3, :fill=>color)
    xy = [339, 204, 376, 253] # Below the ball holder
    TkcRectangle.new(@canvas, xy, :outline=>@C['fg'], :width=>3,
                     :fill=>color, :tag=>'I6c')
    xy = box(346, 339, 28)
    TkcOval.new(@canvas, xy, :fill=>color, :outline=>'') # Roter
    TkcArc.new(@canvas, xy, :outline=>@C['fg'], :width=>2, :style=>:arc,
               :start=>80, :extent=>205)
    TkcArc.new(@canvas, xy, :outline=>@C['fg'], :width=>2, :style=>:arc,
               :start=>-41, :extent=>85)

    xy = box(346, 339, 15) # Center of rotor
    TkcOval.new(@canvas, xy, :outline=>@C['fg'], :fill=>@C['fg'], :tag=>'I6m')
    xy = [352, 312, 352, 254, 368, 254, 368, 322] # Top drop to rotor
    TkcPolygon.new(@canvas, xy, :fill=>color, :outline=>'')
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>2)

    xy = [353, 240, 367, 300] # Poke bottom hole
    TkcRectangle.new(@canvas, xy, :fill=>color, :outline=>'')
    xy = [341, 190, 375, 210] # Poke another hole
    TkcRectangle.new(@canvas, xy, :fill=>color, :outline=>'')

    xy = [
      368, 356, 368, 403, 389, 403, 389, 464, 320, 464, 320, 403,
      352, 403, 352, 366
    ]
    TkcPolygon.new(@canvas, xy, :fill=>color, :outline=>'',
                   :width=>2) # Below rotor
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>2)
    xy = box(275, 342, 7) # On/off rotor
    TkcOval.new(@canvas, xy, :outline=>@C['fg'], :fill=>@C['fg'])
    xy = [276, 334, 342, 325] # Fan belt top
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>3)
    xy = [276, 349, 342, 353] # Fan belt bottom
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>3)

    xy = [337, 212, 337, 247] # What the mouse pushes
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>3, :tag=>'I6_')
    xy = [392, 212, 392, 247]
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>3, :tag=>'I6_')
    xy = [337, 230, 392, 230]
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>7, :tag=>'I6_')

    who = -1 # All the balls
    colors = %w(red cyan orange green blue darkblue)
    colors *= 3

    (0..16).each{|i|
      loc = -i
      color = colors[i]
      x, y = @XY6["#{loc}"]
      TkcOval.new(@canvas, box(x, y, 5),
                  :fill=>color, :outline=>color, :tag=>"I6_b#{i}")
    }
    draw6a(12) # The wheel
  end

  def draw6a(beta)
    @canvas.delete('I6_0')
    ox, oy = [346, 339]
    (0..3).each{|i|
      b = beta + i * 45
      x, y = rotate_c(28, 0, 0, 0, b)
      xy = [ox + x, oy + y, ox - x, oy - y]
      TkcLine.new(@canvas, xy, :tag=>'I6_0', :fill=>@C['fg'], :width=>2)
    }
  end

  def move6(step = nil)
    step = get_step(6, step)

    return 0 if step > 62

    if step < 2  # Open gate for balls to drop
      @canvas.move('I6_', -7, 0)
      if step == 1  # Poke a hole
        xy = [348, 226, 365, 240]
        TkcRectangle.new(@canvas, xy, :fill=>@canvas.itemcget('I6c', :fill),
                         :outline=>'')
      end
      return 1
    end

    s = step - 1  # Do the gumball drop dance
    (0..(((s - 1)/3).to_i)).each{|i|
      tag = "I6_b#{i}"
      break if @canvas.find_withtag(tag).empty?
      loc = s - 3*i

      if @XY6["#{loc},#{i}"]
        move_abs(tag, @XY6["#{loc},#{i}"])
      elsif @XY6["#{loc}"]
        move_abs(tag, @XY6["#{loc}"])
      end
    }
    if s % 3 == 1
      first = (s + 2)/3
      i = first
      loop {
        tag = "I6_b#{i}"
        break if @canvas.find_withtag(tag).empty?
        loc = first - i
        move_abs(tag, @XY6["#{loc}"])
        i += 1
      }
    end
    if s >= 3  # Rotate the motor
      idx = s % 3
      draw6a(12 + s * 15)
    end
    return((s == 3)? 3 : 1)
  end

  # On/off switch
  def draw7
    color = @C['7']
    xy = [198, 306, 277, 374]  # Box
    TkcRectangle.new(@canvas, xy, :outline=>@C['fg'], :width=>2,
                     :fill=>color, :tag=>'I7z')
    @canvas.lower('I7z')
    xy = [275, 343, 230, 349]
    TkcLine.new(@canvas, xy, :tag=>'I7', :fill=>@C['fg'], :arrow=>:last,
                :arrowshape=>[23, 23, 8], :width=>6)
    xy = [225, 324]  # On button
    x, y = xy
    TkcOval.new(@canvas, box(x, y, 3), :fill=>@C['fg'], :outline=>@C['fg'])
    xy = [218, 323]  # On text
    font = ['Times Roman', 8]
    TkcText.new(@canvas, xy, :text=>'on', :anchor=>:e,
                :fill=>@C['fg'], :font=>font)
    xy = [225, 350]  # Off button
    x, y = xy
    TkcOval.new(@canvas, box(x, y, 3), :fill=>@C['fg'], :outline=>@C['fg'])
    xy = [218, 349]  # Off text
    TkcText.new(@canvas, xy, :text=>'off', :anchor=>:e,
                :fill=>@C['fg'], :font=>font)
  end

  def move7(step = nil)
    step = get_step(7, step)

    numsteps = 30
    return 0 if step > numsteps
    beta = 30.0 / numsteps
    rotate_item('I7', 275, 343, beta)

    return((step == numsteps)? 3: 1)
  end

  # Electricity to the fan
  def draw8
    sine([271, 248, 271, 306], 5, 8, :tag=>'I8_s', :fill=>@C['8'], :width=>3)
  end

  def move8(step = nil)
    step = get_step(8, step)

    return 0 if step > 3
    if step == 0
      sparkle(anchor('I8_s', :s), 'I8')
      return 1
    elsif step == 1
      move_abs('I8', anchor('I8_s', :c))
    elsif step == 2
      move_abs('I8', anchor('I8_s', :n))
    else
      @canvas.delete('I8')
    end
    return((step == 2)? 3: 1)
  end

  # Fan
  def draw9
    color = @C['9']
    xy = [266, 194, 310, 220]
    TkcOval.new(@canvas, xy, :outline=>color, :fill=>color)
    xy = [280, 209, 296, 248]
    TkcOval.new(@canvas, xy, :outline=>color, :fill=>color)
    xy = [
      288, 249, 252, 249, 260, 240, 280, 234,
      296, 234, 316, 240, 324, 249, 288, 249
    ]
    TkcPolygon.new(@canvas, xy, :fill=>color, :smooth=>true)

    xy = [248, 205, 265, 214, 264, 205, 265, 196]  # Spinner
    TkcPolygon.new(@canvas, xy, :fill=>color)

    xy = [255, 206, 265, 234]  # Fan blades
    TkcOval.new(@canvas, xy, :fill=>'', :outline=>@C['fg'],
                :width=>3, :tag=>'I9_0')
    xy = [255, 176, 265, 204]
    TkcOval.new(@canvas, xy, :fill=>'', :outline=>@C['fg'],
                :width=>3, :tag=>'I9_0')
    xy = [255, 206, 265, 220]
    TkcOval.new(@canvas, xy, :fill=>'', :outline=>@C['fg'],
                :width=>1, :tag=>'I9_1')
    xy = [255, 190, 265, 204]
    TkcOval.new(@canvas, xy, :fill=>'', :outline=>@C['fg'],
                :width=>1, :tag=>'I9_1')
  end

  def move9(step = nil)
    step = get_step(9, step)

    if (step & 1).nonzero?
      @canvas.itemconfigure('I9_0', :width=>4)
      @canvas.itemconfigure('I9_1', :width=>1)
      @canvas.lower('I9_1', 'I9_0')
    else
      @canvas.itemconfigure('I9_0', :width=>1)
      @canvas.itemconfigure('I9_1', :width=>4)
      @canvas.lower('I9_0', 'I9_1')
    end
    return 3 if step == 0
    return 1
  end

  # Boat
  def draw10
    color  = @C['10a']
    color2 = @C['10b']
    xy = [191, 230, 233, 230, 233, 178, 191, 178]  # Sail
    TkcPolygon.new(@canvas, xy, :fill=>color, :width=>3, :outline=>@C['fg'],
                   :tag=>'I10')
    xy = box(209, 204, 31)  # Front
    TkcArc.new(@canvas, xy, :outline=>'', :fill=>color, :style=>:pie,
               :start=>120, :extent=>120, :tag=>'I10')
    TkcArc.new(@canvas, xy, :outline=>@C['fg'], :width=>3, :style=>:arc,
               :start=>120, :extent=>120, :tag=>'I10')
    xy = box(249, 204, 31)  # Back
    TkcArc.new(@canvas, xy, :outline=>'', :fill=>@C['bg'], :width=>3,
               :style=>:pie, :start=>120, :extent=>120, :tag=>'I10')
    TkcArc.new(@canvas, xy, :outline=>@C['fg'], :width=>3, :style=>:arc,
               :start=>120, :extent=>120, :tag=>'I10')

    xy = [200, 171, 200, 249]  # Mast
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>3, :tag=>'I10')
    xy = [159, 234, 182, 234]  # Bow sprit
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>3, :tag=>'I10')
    xy = [180, 234, 180, 251, 220, 251]  # Hull
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>6, :tag=>'I10')

    xy = [92, 255, 221, 255]  # Waves
    sine(xy, 2, 25, :fill=>color2, :width=>1, :tag=>'I10w')

    xy = @canvas.coords('I10w')[4..-5]  # Water
    xy.concat([222, 266, 222, 277, 99, 277])
    TkcPolygon.new(@canvas, xy, :fill=>color2, :outline=>color2)
    xy = [222, 266, 222, 277, 97, 277, 97, 266]  # Water bottom
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>3)

    xy = box(239, 262, 17)
    TkcArc.new(@canvas, xy, :outline=>@C['fg'], :width=>3, :style=>:arc,
               :start=>95, :extent=>103)
    xy = box(76, 266, 21)
    TkcArc.new(@canvas, xy, :outline=>@C['fg'], :width=>3, :style=>:arc,
               :extent=>190)
  end

  def move10(step = nil)
    step = get_step(10, step)

    pos = [
      [195, 212], [193, 212], [190, 212], [186, 212], [181, 212], [176, 212],
      [171, 212], [166, 212], [161, 212], [156, 212], [151, 212], [147, 212],
      [142, 212], [137, 212], [132, 212, :x], [127, 212], [121, 212],
      [116, 212], [111, 212]
    ]

    return 0 if step >= pos.length

    where = pos[step]
    move_abs('I10', where)

    return 3 if where[2] == :x
    return 1
  end

  # 2nd ball drop
  def draw11
    color  = @C['11a']
    color2 = @C['11b']
    xy = [23, 264, 55, 591]  # Color the down tube
    TkcRectangle.new(@canvas, xy, :fill=>color, :outline=>'')
    xy = box(71, 460, 48)    # Color the outer loop
    TkcOval.new(@canvas, xy, :fill=>color, :outline=>'')

    xy = [55, 264, 55, 458]  # Top right side
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>3)
    xy = [55, 504, 55, 591]  # Bottom right side
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>3)
    xy = box(71, 460, 48)    # Outer loop
    TkcArc.new(@canvas, xy, :outline=>@C['fg'], :width=>3, :style=>:arc,
               :start=>110, :extent=>-290, :tag=>'I11i')
    xy = box(71, 460, 16)    # Inner loop
    TkcOval.new(@canvas, xy, :outline=>@C['fg'], :fill=>'',
                :width=>3, :tag=>'I11i')
    TkcOval.new(@canvas, xy, :outline=>@C['fg'], :fill=>@C['bg'], :width=>3)

    xy = [23, 264, 23, 591]  # Left side
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>3)
    xy = box(1, 266, 23)     # Top left curve
    TkcArc.new(@canvas, xy, :outline=>@C['fg'], :width=>3,
               :style=>:arc, :extent=>90)

    xy = box(75, 235, 9)     # The ball
    TkcOval.new(@canvas, xy, :fill=>color2, :outline=>'',
                :width=>3, :tag=>'I11')
  end

  def move11(step = nil)
    step = get_step(11, step)

    pos = [
      [75, 235], [70, 235], [65, 237], [56, 240], [46, 247], [38, 266],
      [38, 296], [38, 333], [38, 399], [38, 475], [74, 496], [105, 472],
      [100, 437], [65, 423], [-100, -100], [38, 505], [38, 527, :x], [38, 591]
    ]

    return 0 if step >= pos.length
    where = pos[step]
    move_abs('I11', where)
    return 3 if where[2] == :x
    return 1
  end

  # Hand
  def draw12
    xy = [
      20, 637, 20, 617, 20, 610, 20, 590, 40, 590, 40, 590,
      60, 590, 60, 610, 60, 610
    ]
    xy.concat([60, 610, 65, 620, 60, 631])  # Thumb
    xy.concat([60, 631, 60, 637, 60, 662, 60, 669, 52, 669,
                56, 669, 50, 669, 50, 662, 50, 637])

    y0 = 637  # Bumps for fingers
    y1 = 645
    50.step(21, -10){|x|
      x1 = x - 5
      x2 = x - 10
      xy << x << y0 << x1 << y1 << x2 << y0
    }
    TkcPolygon.new(@canvas, xy, :fill=>@C['12'], :outline=>@C['fg'],
                   :smooth=>true, :tag=>'I12', :width=>3)
  end

  def move12(step = nil)
    step = get_step(12, step)

    pos = [[42.5, 641, :x]]
    return 0 if step >= pos.length
    where = pos[step]
    move_abs('I12', where)
    return 3 if where[2] == :x
    return 1
  end

  # Fax
  def draw13
    color = @C['13a']
    xy = [86, 663, 149, 663, 149, 704, 50, 704, 50, 681, 64, 681, 86, 671]
    xy2 = [
      784, 663, 721, 663, 721, 704, 820, 704, 820, 681, 806, 681, 784, 671
    ]
    radii = [2, 9, 9, 8, 5, 5, 2]

    round_poly(@canvas, xy, radii, :width=>3,
               :outline=>@C['fg'], :fill=>color)
    round_poly(@canvas, xy2, radii, :width=>3,
               :outline=>@C['fg'], :fill=>color)

    xy = [56, 677]
    x, y = xy
    TkcRectangle.new(@canvas, box(x, y, 4), :fill=>'', :outline=>@C['fg'],
                     :width=>3, :tag=>'I13')
    xy = [809, 677]
    x, y = xy
    TkcRectangle.new(@canvas, box(x, y, 4), :fill=>'', :outline=>@C['fg'],
                     :width=>3, :tag=>'I13R')

    xy = [112, 687]  # Label
    TkcText.new(@canvas, xy, :text=>'FAX', :fill=>@C['fg'],
                :font=>['Times Roman', 12, :bold])
    xy = [762, 687]
    TkcText.new(@canvas, xy, :text=>'FAX', :fill=>@C['fg'],
                :font=>['Times Roman', 12, :bold])

    xy = [138, 663, 148, 636, 178, 636]  # Paper guide
    TkcLine.new(@canvas, xy, :smooth=>true, :fill=>@C['fg'], :width=>3)
    xy = [732, 663, 722, 636, 692, 636]
    TkcLine.new(@canvas, xy, :smooth=>true, :fill=>@C['fg'], :width=>3)

    sine([149, 688, 720, 688], 5, 15,
         :tag=>'I13_s', :fill=>@C['fg'],  :width=>3)
  end

  def move13(step = nil)
    step = get_step(13, step)

    numsteps = 7

    if step == numsteps + 2
      move_abs('I13_star', [-100, -100])
      @canvas.itemconfigure('I13R', :fill=>@C['13b'], :width=>2)
      return 2
    end
    if step == 0  # Button down
      @canvas.delete('I13')
      sparkle([-100, -100], 'I13_star')  # Create off screen
      return 1
    end
    x0, y0 = anchor('I13_s', :w)
    x1, y1 = anchor('I13_s', :e)
    x = x0 + (x1 - x0) * (step - 1) / numsteps.to_f
    move_abs('I13_star', [x, y0])
    return 1
  end

  # Paper in fax
  def draw14
    color = @C['14']
    xy = [102, 661, 113, 632, 130, 618]  # Left paper edge
    TkcLine.new(@canvas, xy, :smooth=>true, :fill=>color,
                :width=>3, :tag=>'I14L_0')
    xy = [148, 629, 125, 640, 124, 662]  # Right paper edge
    TkcLine.new(@canvas, xy, :smooth=>true, :fill=>color,
                :width=>3, :tag=>'I14L_1')
    draw14a('L')

    xy = [
      768.0, 662.5, 767.991316225, 662.433786215, 767.926187912, 662.396880171
    ]
    TkcLine.new(@canvas, xy, :smooth=>true, :fill=>color,
                :width=>3, :tag=>'I14R_0')
    @canvas.lower('I14R_0')
    # NB. these numbers are VERY sensitive, you must start with final size
    # and shrink down to get the values
    xy = [
      745.947897349, 662.428358855, 745.997829056, 662.452239237, 746.0, 662.5
    ]
    TkcLine.new(@canvas, xy, :smooth=>true, :fill=>color,
                :width=>3, :tag=>'I14R_1')
    @canvas.lower('I14R_1')
  end

  def draw14a(side)
    color = @C['14']
    xy = @canvas.coords("I14#{side}_0")
    xy2 = @canvas.coords("I14#{side}_1")
    x0, y0, x1, y1, x2, y2 = xy
    x3, y3, x4, y4, x5, y5 = xy2

    zz = [
      x0, y0, x0, y0, xy, x2, y2, x2, y2,
      x3, y3, x3, y3, xy2, x5, y5, x5, y5
    ].flatten
    @canvas.delete("I14#{side}")
    TkcPolygon.new(@canvas, zz, :tag=>"I14#{side}", :smooth=>true,
                   :fill=>color, :outline=>color, :width=>3)
    @canvas.lower("I14#{side}")
  end

  def move14(step = nil)
    step = get_step(14, step)

    # Paper going down
    sc = 0.9 - 0.05*step
    if sc < 0.3
      @canvas.delete('I14L')
      return 0
    end

    ox, oy = @canvas.coords('I14L_0')
    @canvas.scale('I14L_0', ox, oy, sc, sc)
    ox, oy = @canvas.coords('I14L_1')[-2..-1]
    @canvas.scale('I14L_1', ox, oy, sc, sc)
    draw14a('L')

    # Paper going up
    sc = 0.35 + 0.05*step
    sc = 1/sc

    ox, oy = @canvas.coords('I14R_0')
    @canvas.scale('I14R_0', ox, oy, sc, sc)
    ox, oy = @canvas.coords('I14R_1')[-2..-1]
    @canvas.scale('I14R_1', ox, oy, sc, sc)
    draw14a('R')

    return((step == 10)? 3: 1)
  end

  # Light beam
  def draw15
    color = @C['15a']
    xy = [824, 599, 824, 585, 820, 585, 829, 585]
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>3, :tag=>'I15a')
    xy = [789, 599, 836, 643]
    TkcRectangle.new(@canvas, xy, :fill=>color, :outline=>@C['fg'], :width=>3)
    xy = [778, 610, 788, 632]
    TkcRectangle.new(@canvas, xy, :fill=>color, :outline=>@C['fg'], :width=>3)
    xy = [766, 617, 776, 625]
    TkcRectangle.new(@canvas, xy, :fill=>color, :outline=>@C['fg'], :width=>3)

    xy = [633, 600, 681, 640]
    TkcRectangle.new(@canvas, xy, :fill=>color, :outline=>@C['fg'], :width=>3)
    xy = [635, 567, 657, 599]
    TkcRectangle.new(@canvas, xy, :fill=>color, :outline=>@C['fg'], :width=>2)
    xy = [765, 557, 784, 583]
    TkcRectangle.new(@canvas, xy, :fill=>color, :outline=>@C['fg'], :width=>2)

    sine([658, 580, 765, 580], 3, 15,
         :tag=>'I15_s', :fill=>@C['fg'], :width=>3)
  end

  def move15a
    color = @C['15b']
    @canvas.scale('I15a', 824, 599, 1, 0.3)  # Button down
    xy = [765, 621, 681, 621]
    TkcLine.new(@canvas, xy, :dash=>'-', :width=>3, :fill=>color, :tag=>'I15')
  end

  def move15(step = nil)
    step = get_step(15, step)

    numsteps = 6

    if step == numsteps + 2
      move_abs('I15_star', [-100, -100])
      return 2
    end
    if step == 0  # Break the light beam
      sparkle([-100, -100], 'I15_star')
      xy = [765, 621, 745, 621]
      @canvas.coords('I15', xy)
      return 1
    end
    x0, y0 = anchor('I15_s', :w)
    x1, y1 = anchor('I15_s', :e)
    x = x0 + (x1 - x0) * (step - 1) / numsteps.to_f
    move_abs('I15_star', [x, y0])
    return 1
  end

  # Bell
  def draw16
    color = @C['16']
    xy = [722, 485, 791, 556]
    TkcRectangle.new(@canvas, xy, :fill=>'', :outline=>@C['fg'], :width=>3)
    xy = box(752, 515, 25)  # Bell
    TkcOval.new(@canvas, xy, :fill=>color, :outline=>'black',
                :tag=>'I16b', :width=>2)
    xy = box(752, 515, 5)   # Bell button
    TkcOval.new(@canvas, xy, :fill=>'black', :outline=>'black', :tag=>'I16b')

    xy = [784, 523, 764, 549]  # Clapper
    TkcLine.new(@canvas, xy, :width=>3, :tag=>'I16c', :fill=>@C['fg'])
    xy = box(784, 523, 4)
    TkcOval.new(@canvas, xy, :fill=>@C['fg'], :outline=>@C['fg'], :tag=>'I16d')
  end

  def move16(step = nil)
    step = get_step(16, step)

    # Note: we never stop
    ox, oy = [760, 553]
    if (step & 1).nonzero?
      beta = 12
      @canvas.move('I16b', 3, 0)
    else
      beta = -12
      @canvas.move('I16b', -3, 0)
    end
    rotate_item('I16c', ox, oy, beta)
    rotate_item('I16d', ox, oy, beta)

    return ((step == 1)? 3: 1)
  end

  # Cat
  def draw17
    color = @C['17']

    xy = [584, 556, 722, 556]
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>3)
    xy = [584, 485, 722, 485]
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>3)

    xy = [664, 523, 717, 549]  # Body
    TkcArc.new(@canvas, xy, :outline=>@C['fg'], :fill=>color, :width=>3,
               :style=>:chord, :start=>128, :extent=>260, :tag=>'I17')

    xy = [709, 554, 690, 543]  # Paw
    TkcOval.new(@canvas, xy, :outline=>@C['fg'], :fill=>color,
                :width=>3, :tag=>'I17')
    xy = [657, 544, 676, 555]
    TkcOval.new(@canvas, xy, :outline=>@C['fg'], :fill=>color,
                :width=>3, :tag=>'I17')

    xy = box(660, 535, 15)     # Lower face
    TkcArc.new(@canvas, xy, :outline=>@C['fg'], :width=>3, :style=>:arc,
               :start=>150, :extent=>240, :tag=>'I17_')
    TkcArc.new(@canvas, xy, :outline=>'', :fill=>color, :width=>1,
               :style=>:chord, :start=>150, :extent=>240, :tag=>'I17_')
    xy = [674, 529, 670, 513, 662, 521, 658, 521, 650, 513, 647, 529]  # Ears
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>3, :tag=>'I17_')
    TkcPolygon.new(@canvas, xy, :fill=>color, :outline=>'', :width=>1,
                   :tag=>['I17_', 'I17_c'])
    xy = [652, 542, 628, 539]  # Whiskers
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>3, :tag=>'I17_')
    xy = [652, 543, 632, 545]
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>3, :tag=>'I17_')
    xy = [652, 546, 632, 552]
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>3, :tag=>'I17_')

    xy = [668, 543, 687, 538]
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>3,
                :tag=>['I17_', 'I17_w'])
    xy = [668, 544, 688, 546]
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>3,
                :tag=>['I17_', 'I17_w'])
    xy = [668, 547, 688, 553]
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>3,
                :tag=>['I17_', 'I17_w'])

    xy = [649, 530, 654, 538, 659, 530]  # Left eye
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>2,
                :smooth=>true, :tag=>'I17')
    xy = [671, 530, 666, 538, 661, 530]  # Right eye
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>2,
                :smooth=>true, :tag=>'I17')
    xy = [655, 543, 660, 551, 665, 543]  # Mouth
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>2,
                :smooth=>true, :tag=>'I17')
  end

  def move17(step = nil)
    step = get_step(17, step)

    if step == 0
      @canvas.delete('I17')  # Delete most of the cat
      xy = [655, 543, 660, 535, 665, 543]  # Mouth
      TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>3,
                  :smooth=>true, :tag=>'I17_')
      xy = box(654, 530, 4)  # Left eye
      TkcOval.new(@canvas, xy, :outline=>@C['fg'], :width=>3, :fill=>'',
                  :tag=>'I17_')
      xy = box(666, 530, 4)  # Right eye
      TkcOval.new(@canvas, xy, :outline=>@C['fg'], :width=>3, :fill=>'',
                  :tag=>'I17_')

      @canvas.move('I17_', 0, -20) # Move face up
      xy = [652, 528, 652, 554]    # Front leg
      TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>3, :tag=>'I17_')
      xy = [670, 528, 670, 554]    # 2nd front leg
      TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>3, :tag=>'I17_')

      xy = [ # Body
        675, 506, 694, 489, 715, 513, 715, 513, 715, 513, 716, 525,
        716, 525, 716, 525, 706, 530, 695, 530, 679, 535, 668, 527,
        668, 527, 668, 527, 675, 522, 676, 517, 677, 512
      ]
      TkcPolygon.new(@canvas, xy, :fill=>@canvas.itemcget('I17_c', :fill),
                     :outline=>@C['fg'], :width=>3, :smooth=>true,
                     :tag=>'I17_')
      xy = [716, 514, 716, 554]  # Back leg
      TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>3, :tag=>'I17_')
      xy = [694, 532, 694, 554]  # 2nd back leg
      TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>3, :tag=>'I17_')
      xy = [715, 514, 718, 506, 719, 495, 716, 488]  # Tail
      TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>3,
                  :smooth=>true, :tag=>'I17_')

      @canvas.raise('I17w')       # Make whiskers visible
      @canvas.move('I17_', -5, 0) # Move away from the wall a bit
      return 2
    end
    return 0
  end

  # Sling shot
  def draw18
    color = @C['18']
    xy = [721, 506, 627, 506]  # Sling hold
    TkcLine.new(@canvas, xy, :width=>4, :fill=>@C['fg'], :tag=>'I18')

    xy = [607, 500, 628, 513]  # Sling rock
    TkcOval.new(@canvas, xy, :fill=>color, :outline=>'', :tag=>'I18a')

    xy = [526, 513, 606, 507, 494, 502]  # Sling band
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>4, :tag=>'I18b')
    xy = [485, 490, 510, 540, 510, 575, 510, 540, 535, 491]  # Sling
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>6)
  end

  def move18(step = nil)
    step = get_step(18, step)

    pos = [
      [587, 506], [537, 506], [466, 506], [376, 506], [266, 506, :x],
      [136, 506], [16, 506], [-100, -100]
    ]

    b = []
    b[0] = [490, 502, 719, 507, 524, 512]  # Band collapsing
    b[1] = [
      491, 503, 524, 557, 563, 505, 559, 496, 546, 506, 551, 525,
      553, 536, 538, 534, 532, 519, 529, 499
    ]
    b[2] = [
      491, 503, 508, 563, 542, 533, 551, 526, 561, 539, 549, 550, 530, 500
    ]
    b[3] = [
      491, 503, 508, 563, 530, 554, 541, 562, 525, 568, 519, 544, 530, 501
    ]

    return 0 if step >= pos.length

    if step == 0
      @canvas.delete('I18')
      @canvas.itemconfigure('I18b', :smooth=>true)
    end
    if b[step]
      @canvas.coords('I18b', b[step])
    end

    where = pos[step]
    move_abs('I18a', where)
    return 3 if where[2] == :x
    return 1
  end

  # Water pipe
  def draw19
    color = @C['19']
    xx = [[249, 181], [155, 118], [86, 55], [22, 0]]
    xx.each{|x1, x2|
      TkcRectangle.new(@canvas, x1, 453, x2, 467,
                       :fill=>color, :outline=>'', :tag=>'I19')
      TkcLine.new(@canvas, x1, 453, x2, 453,
                  :fill=>@C['fg'], :width=>1) # Pipe top
      TkcLine.new(@canvas, x1, 467, x2, 467,
                  :fill=>@C['fg'], :width=>1) # Pipe bottom
    }
    @canvas.raise('I11i')

    xy = box(168, 460, 16)  # Bulge by the joint
    TkcOval.new(@canvas, xy, :fill=>color, :outline=>'')
    TkcArc.new(@canvas, xy, :outline=>@C['fg'], :width=>1, :style=>:arc,
               :start=>21, :extent=>136)
    TkcArc.new(@canvas, xy, :outline=>@C['fg'], :width=>1, :style=>:arc,
               :start=>-21, :extent=>-130)

    xy = [249, 447, 255, 473]  # First joint 26x6
    TkcRectangle.new(@canvas, xy, :fill=>color, :outline=>@C['fg'], :width=>1)

    xy = box(257, 433, 34)     # Bend up
    TkcArc.new(@canvas, xy, :outline=>'', :fill=>color, :width=>1,
               :style=>:pie, :start=>0, :extent=>-91)
    TkcArc.new(@canvas, xy, :outline=>@C['fg'], :width=>1,
               :style=>:arc, :start=>0, :extent=>-90)
    xy = box(257, 433, 20)
    TkcArc.new(@canvas, xy, :outline=>'', :fill=>@C['bg'], :width=>1,
               :style=>:pie, :start=>0, :extent=>-92)
    TkcArc.new(@canvas, xy, :outline=>@C['fg'], :width=>1,
               :style=>:arc, :start=>0, :extent=>-90)
    xy = box(257, 421, 34)     # Bend left
    TkcArc.new(@canvas, xy, :outline=>'', :fill=>color, :width=>1,
               :style=>:pie, :start=>0, :extent=>91)
    TkcArc.new(@canvas, xy, :outline=>@C['fg'], :width=>1,
               :style=>:arc, :start=>0, :extent=>90)
    xy = box(257, 421, 20)
    TkcArc.new(@canvas, xy, :outline=>'', :fill=>@C['bg'], :width=>1,
               :style=>:pie, :start=>0, :extent=>90)
    TkcArc.new(@canvas, xy, :outline=>@C['fg'], :width=>1,
               :style=>:arc, :start=>0, :extent=>90)
    xy = box(243, 421, 34)     # Bend down
    TkcArc.new(@canvas, xy, :outline=>'', :fill=>color, :width=>1,
               :style=>:pie, :start=>90, :extent=>90)
    TkcArc.new(@canvas, xy, :outline=>@C['fg'], :width=>1,
               :style=>:arc, :start=>90, :extent=>90)
    xy = box(243, 421, 20)
    TkcArc.new(@canvas, xy, :outline=>'', :fill=>@C['bg'], :width=>1,
               :style=>:pie, :start=>90, :extent=>90)
    TkcArc.new(@canvas, xy, :outline=>@C['fg'], :width=>1,
               :style=>:arc, :start=>90, :extent=>90)

    xy = [270, 427, 296, 433]  # 2nd joint bottom
    TkcRectangle.new(@canvas, xy, :fill=>color, :outline=>@C['fg'], :width=>1)
    xy = [270, 421, 296, 427]  # 2nd joint top
    TkcRectangle.new(@canvas, xy, :fill=>color, :outline=>@C['fg'], :width=>1)
    xy = [249, 382, 255, 408]  # Third joint right
    TkcRectangle.new(@canvas, xy, :fill=>color, :outline=>@C['fg'], :width=>1)
    xy = [243, 382, 249, 408]  # Third joint left
    TkcRectangle.new(@canvas, xy, :fill=>color, :outline=>@C['fg'], :width=>1)
    xy = [203, 420, 229, 426]  # Last joint
    TkcRectangle.new(@canvas, xy, :fill=>color, :outline=>@C['fg'], :width=>1)

    xy = box(168, 460, 6)      # Handle joint
    TkcOval.new(@canvas, xy, :fill=>@C['fg'], :outline=>'', :tag=>'I19a')
    xy = [168, 460, 168, 512]  # Handle bar
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>5, :tag=>'I19b')
  end

  def move19(step = nil)
    step = get_step(19, step)

     angles = [30, 30, 30]
    return 2 if step == angles.length
    ox, oy = centroid('I19a')
    rotate_item('I19b', ox, oy, angles[step])

    return 1
  end

  # Water pouring
  def draw20
    # do nothing
  end

  def move20(step = nil)
    step = get_step(20, step)

    pos  = [451, 462, 473, 484, 496, 504, 513, 523, 532]
    freq  = [20,  40,  40,  40,  40,  40,  40,  40,  40]
    pos = [
      [451, 20], [462, 40], [473, 40], [484, 40], [496, 40],
      [504, 40], [513, 40], [523, 40], [532, 40, :x]
    ]
    return 0 if step >= pos.length

    @canvas.delete('I20')
    where = pos[step]
    y, f = where
    h20(y, f)
    return 3 if where[2] == :x
    return 1
  end

  def h20(y, f)
    color = @C['20']
    @canvas.delete('I20')

    sine([208, 428, 208, y], 4, f, :tag=>['I20', 'I20s'],
         :width=>3, :fill=>color, :smooth=>true)
    TkcLine.new(@canvas, @canvas.coords('I20s'), :width=>3,
                :fill=>color, :smooth=>1, :tag=>['I20', 'I20a'])
    TkcLine.new(@canvas, @canvas.coords('I20s'), :width=>3,
                :fill=>color, :smooth=>1, :tag=>['I20', 'I20b'])
    @canvas.move('I20a', 8, 0)
    @canvas.move('I20b', 16, 0)
  end

  # Bucket
  def draw21
    color = @C['21']
    xy = [217, 451, 244, 490]  # Right handle
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>2, :tag=>'I21_a')
    xy = [201, 467, 182, 490]  # Left handle
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>2, :tag=>'I21_a')

    xy = [245, 490, 237, 535]  # Right side
    xy2 = [189, 535, 181, 490] # Left side
    TkcPolygon.new(@canvas, xy + xy2, :fill=>color, :outline=>'',
                   :tag=>['I21', 'I21f'])
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>2, :tag=>'I21')
    TkcLine.new(@canvas, xy2, :fill=>@C['fg'], :width=>2, :tag=>'I21')

    xy = [182, 486, 244, 498]  # Top
    TkcOval.new(@canvas, xy, :fill=>color, :outline=>'', :width=>2,
                :tag=>['I21', 'I21f'])
    TkcOval.new(@canvas, xy, :fill=>'', :outline=>@C['fg'], :width=>2,
                :tag=>['I21', 'I21t'])
    xy = [189, 532, 237, 540]  # Bottom
    TkcOval.new(@canvas, xy, :fill=>color, :outline=>@C['fg'], :width=>2,
                :tag=>['I21', 'I21b'])
  end

  def move21(step = nil)
    step = get_step(21, step)

    numsteps = 30
    return 0 if step >= numsteps

    x1, y1, x2, y2 = @canvas.coords('I21b')
    # lx1, ly1, lx2, ly2 = @canvas.coords('I21t')
    lx1, ly1, lx2, ly2 = [183, 492, 243, 504]

    f = step / numsteps.to_f
    y2 = y2 - 3
    xx1 = x1 + (lx1 - x1) * f
    yy1 = y1 + (ly1 - y1) * f
    xx2 = x2 + (lx2 - x2) * f
    yy2 = y2 + (ly2 - y2) * f

    @canvas.itemconfigure('I21b', :fill=>@C['20'])
    @canvas.delete('I21w')
    TkcPolygon.new(@canvas, x2, y2, x1, y1, xx1, yy1, xx2, yy1,
                   :tag=>['I21', 'I21w'], :outline=>'', :fill=>@C['20'])
    @canvas.lower('I21w', 'I21')
    @canvas.raise('I21b')
    @canvas.lower('I21f')

    return((step == numsteps - 1)? 3: 1)
  end

  # Bucket drop
  def draw22
    # do nothing
  end

  def move22(step = nil)
    step = get_step(22, step)
    pos = [[213, 513], [213, 523], [213, 543, :x], [213, 583], [213, 593]]

    @canvas.itemconfigure('I21f', :fill=>@C['22']) if step == 0
    return 0 if step >= pos.length
    where = pos[step]
    move_abs('I21', where)
    h20(where[1], 40)
    @canvas.delete('I21_a')  # Delete handles

    return 3 if where[2] == :x
    return 1
  end

  # Blow dart
  def draw23
    color  = @C['23a']
    color2 = @C['23b']
    color3 = @C['23c']

    xy = [185, 623, 253, 650]  # Block
    TkcRectangle.new(@canvas, xy, :fill=>'black', :outline=>@C['fg'],
                     :width=>2, :tag=>'I23a')
    xy = [187, 592, 241, 623]  # Balloon
    TkcOval.new(@canvas, xy, :outline=>'', :fill=>color, :tag=>'I23b')
    TkcArc.new(@canvas, xy, :outline=>@C['fg'], :width=>3, :tag=>'I23b',
               :style=>:arc, :start=>12, :extent=>336)
    xy = [239, 604, 258, 589, 258, 625, 239, 610]  # Balloon nozzle
    TkcPolygon.new(@canvas, xy, :outline=>'', :fill=>color, :tag=>'I23b')
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>3, :tag=>'I23b')

    xy = [285, 611, 250, 603]  # Dart body
    TkcOval.new(@canvas, xy, :fill=>color2, :outline=>@C['fg'],
                :width=>3, :tag=>'I23d')
    xy = [249, 596, 249, 618, 264, 607, 249, 596]  # Dart tail
    TkcPolygon.new(@canvas, xy, :fill=>color3, :outline=>@C['fg'],
                   :width=>3, :tag=>'I23d')
    xy = [249, 607, 268, 607]  # Dart detail
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>3, :tag=>'I23d')
    xy = [285, 607, 305, 607]  # Dart needle
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>3, :tag=>'I23d')
  end

  def move23(step = nil)
    step = get_step(23, step)

    pos = [
      [277, 607], [287, 607], [307, 607, :x], [347, 607], [407, 607],
      [487, 607], [587, 607], [687, 607], [787, 607], [-100, -100]
    ]

    return 0 if step >= pos.length
    if step <= 1
      ox, oy = anchor('I23a', :n)
      @canvas.scale('I23b', ox, oy, 0.9, 0.5)
    end
    where = pos[step]
    move_abs('I23d', where)

    return 3 if where[2] == :x
    return 1
  end

  # Balloon
  def draw24
    color = @C['24a']
    xy = [366, 518, 462, 665]  # Balloon
    TkcOval.new(@canvas, xy, :fill=>color, :outline=>@C['fg'],
                :width=>3, :tag=>'I24')
    xy = [414, 666, 414, 729]  # String
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :width=>3, :tag=>'I24')
    xy = [410, 666, 404, 673, 422, 673, 418, 666]  # Nozzle
    TkcPolygon.new(@canvas, xy, :fill=>color, :outline=>@C['fg'],
                   :width=>3, :tag=>'I24')

    xy = [387, 567, 390, 549, 404, 542]  # Reflections
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :smooth=>true,
                :width=>2, :tag=>'I24')
    xy = [395, 568, 399, 554, 413, 547]
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :smooth=>true,
                :width=>2, :tag=>'I24')
    xy = [403, 570, 396, 555, 381, 553]
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :smooth=>true,
                :width=>2, :tag=>'I24')
    xy = [408, 564, 402, 547, 386, 545]
    TkcLine.new(@canvas, xy, :fill=>@C['fg'], :smooth=>true,
                :width=>2, :tag=>'I24')
  end

  def move24(step = nil)
    step = get_step(24, step)

    return 0 if step > 4
    return 2 if step == 4

    if step == 0
      @canvas.delete('I24')  # Exploding balloon
      xy = [
        347, 465, 361, 557, 271, 503, 272, 503, 342, 574, 259, 594,
        259, 593, 362, 626, 320, 737, 320, 740, 398, 691, 436, 738,
        436, 739, 476, 679, 528, 701, 527, 702, 494, 627, 548, 613,
        548, 613, 480, 574, 577, 473, 577, 473, 474, 538, 445, 508,
        431, 441, 431, 440, 400, 502, 347, 465, 347, 465
      ]
      TkcPolygon.new(@canvas, xy, :tag=>'I24', :fill=>@C['24b'],
                     :outline=>@C['24a'], :width=>10, :smooth=>true)
      msg = Tk.subst(@S['message'].value)
      TkcText.new(@canvas, centroid('I24'), :text=>msg, :tag=>['I24', 'I24t'],
                  :justify=>:center, :font=>['Times Roman', 18, :bold])
      return 1
    end

    @canvas.itemconfigure('I24t', :font=>['Times Roman', 18 + 6*step, :bold])
    @canvas.move('I24', 0, -60)
    ox, oy = centroid('I24')
    @canvas.scale('I24', ox, oy, 1.25, 1.25)
    return 1
  end

  # Displaying the message
  def move25(step = nil)
    step = get_step(25, step)

    if step == 0
      @XY['25'] = Tk::Clock.clicks(:miliseconds)
      return 1
    end
    elapsed = Tk::Clock.clicks(:miliseconds) - @XY['25']
    return 1 if elapsed < 5000
    return 2
  end

  # Collapsing balloon
  def move26(step = nil)
    step = get_step(26, step)

    if step >= 3
      @canvas.delete('I24', 'I26')
      TkcText.new(@canvas, 430, 740, :anchor=>:s, :tag=>'I26',
                  :text=>'click to continue',
                  :font=>['Times Roman', 24, :bold])
      @canvas.bind('1', proc{reset})
      return 4
    end

    ox, oy = centroid('I24')
    @canvas.scale('I24', ox, oy, 0.8, 0.8)
    @canvas.move('I24', 0, 60)
    @canvas.itemconfigure('I24t', :font=>['Times Roman', 30 - 6*step, :bold])
    return 1
  end

  ################################################################
  #
  # Helper functions
  #
  def box(x, y, r)
    [x - r, y - r, x + r, y + r]
  end

  def move_abs(item, xy)
    x, y = xy
    ox, oy = centroid(item)
    dx = x - ox
    dy = y - oy
    @canvas.move(item, dx, dy)
  end

  def rotate_item(item, ox, oy, beta)
    xy = @canvas.coords(item)
    xy2 = []
    0.step(xy.length - 1, 2){|idx|
      x, y = xy[idx, 2]
      xy2.concat(rotate_c(x, y, ox, oy, beta))
    }
    @canvas.coords(item, xy2)
  end

  def rotate_c(x, y, ox, oy, beta)
    # rotates vector (ox,oy)->(x,y) by beta degrees clockwise

    x -= ox    # Shift to origin
    y -= oy

    beta = beta * Math.atan(1) * 4 / 180.0        # Radians
    xx = x * Math.cos(beta) - y * Math.sin(beta)  # Rotate
    yy = x * Math.sin(beta) + y * Math.cos(beta)

    xx += ox  # Shift back
    yy += oy

    [xx, yy]
  end

  def reset
    draw_all
    @canvas.bind_remove('1')
    @S['mode'].value = :MSTART
    @S['active'] = [0]
  end

  # Each Move## keeps its state info in STEP, this retrieves and increments it
  def get_step(who, step)
    if step
      @STEP[who] = step
    else
      if !@STEP.exist?(who) || @STEP[who] == ""
        @STEP[who] = 0
      else
        @STEP[who] += 1
      end
    end
    @STEP[who]
  end

  def reset_step
    @S['cnt'].value = 0
    @STEP.keys.each{|k| @STEP[k] = ''}
  end

  def sine(xy0, amp, freq, opts = {})
    x0, y0, x1, y1 = xy0
    step = 2
    xy = []
    if y0 == y1  # Horizontal
      x0.step(x1, step){|x|
        beta = (x - x0) * 2 * Math::PI / freq
        y = y0 + amp * Math.sin(beta)
        xy << x << y
      }
    else
      y0.step(y1, step){|y|
        beta = (y - y0) * 2 * Math::PI / freq
        x = x0 + amp * Math.sin(beta)
        xy << x << y
      }
    end
    TkcLine.new(@canvas, xy, opts)
  end

  def round_rect(xy, radius, opts={})
    x0, y0, x3, y3 = xy
    r = @canvas.winfo_pixels(radius)
    d = 2 * r

    # Make sure that the radius of the curve is less than 3/8 size of the box!
    maxr = 0.75
    if d > maxr * (x3 - x0)
      d = maxr * (x3 - x0)
    end
    if d > maxr * (y3 - y0)
      d = maxr * (y3 - y0)
    end

    x1 = x0 + d
    x2 = x3 - d
    y1 = y0 + d
    y2 = y3 - d

    xy = [x0, y0, x1, y0, x2, y0, x3, y0, x3, y1, x3, y2]
    xy.concat([x3, y3, x2, y3, x1, y3, x0, y3, x0, y2, x0, y1])
    return xy
  end

  def round_poly(canv, xy, radii, opts)
    lenXY = xy.length
    lenR = radii.length
    if lenXY != 2*lenR
      raise "wrong number of vertices and radii"
    end

    knots = []
    x0 = xy[-2]; y0 = xy[-1]
    x1 = xy[0];  y1 = xy[1]
    xy << xy[0] << xy[1]

    0.step(lenXY - 1, 2){|i|
      radius = radii[i/2]
      r = canv.winfo_pixels(radius)

      x2 = xy[i+2];  y2 = xy[i+3]
      z = _round_poly2(x0, y0, x1, y1, x2, y2, r)
      knots.concat(z)

      x0 = x1;  y0 = y1
      x1 = x2;  y1 = y2
    }
    TkcPolygon.new(canv, knots, {:smooth=>true}.update(opts))
  end

  def _round_poly2(x0, y0, x1, y1, x2, y2, radius)
    d = 2 * radius
    maxr = 0.75

    v1x = x0 - x1
    v1y = y0 - y1
    v2x = x2 - x1
    v2y = y2 - y1

    vlen1 = Math.sqrt(v1x*v1x + v1y*v1y)
    vlen2 = Math.sqrt(v2x*v2x + v2y*v2y)

    if d > maxr * vlen1
      d = maxr * vlen1
    end
    if d > maxr * vlen2
      d = maxr * vlen2
    end

    xy = []
    xy << (x1 + d * v1x / vlen1) << (y1 + d * v1y / vlen1)
    xy << x1 << y1
    xy << (x1 + d * v2x / vlen2) << (y1 + d * v2y / vlen2)

    return xy
  end

  def sparkle(oxy, tag)
    xy = [
      [299, 283], [298, 302], [295, 314], [271, 331],
      [239, 310], [242, 292], [256, 274], [281, 273]
    ]
    xy.each{|x, y|
      TkcLine.new(@canvas, 271, 304, x, y,
                  :fill=>'white', :width=>3, :tag=>tag)
    }
    move_abs(tag, oxy)
  end

  def centroid(item)
    anchor(item, :c)
  end

  def anchor(item, where)
    x1, y1, x2, y2 = @canvas.bbox(item)
    case(where)
    when :n
      y = y1
    when :s
      y = y2
    else
      y = (y1 + y2) / 2.0
    end
    case(where)
    when :w
      x = x1
    when :e
      x = x2
    else
      x = (x1 + x2) / 2.0
    end
    return [x, y]
  end
end

TkGoldberg_Demo.new(base_frame)
