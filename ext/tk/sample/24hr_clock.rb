#!/usr/bin/env ruby

require 'tk'

class Clock
  def initialize(clock24 = true)
    @clock = (clock24)? 24: 12

    @size = 200
    @cdot_size = 5

    @cdot_color        = 'black'
    @hour_hand_color   = 'black'
    @minute_hand_color = 'gray25'
    @second_hand_color = 'gray50'

    @mark_font     = 'Helvetica -14'
    @mark_width    = 3
    @mark_color    = 'black'
    @submark_color = 'gray50'

    @c = TkCanvas.new(:width=>2*@size, :height=>2*@size,
                     :scrollregion=>[-@size, -@size, @size, @size]
                     ).pack(:fill=>:both, :expand=>true)

    @tag = TkcTag.new(@c)
    @hand_tag = TkcTag.new(@c)

    @circle_coords = [[-0.9*@size, -0.9*@size], [0.9*@size, 0.9*@size]]
    @oval = TkcOval.new(@c, @circle_coords, :fill=>'white', :tags=>[@tag])

    f = TkFrame.new.pack
    TkLabel.new(f, :text=>'CURRENT:').pack(:side=>:left)
    @now = TkLabel.new(f, :text=>'00:00:00').pack(:side=>:left, :padx=>2)
    TkLabel.new(f, :text=>'  ').pack(:side=>:left)
    TkLabel.new(f, :text=>'  ').pack(:side=>:right)
    @l = TkLabel.new(f, :text=>'00:00').pack(:side=>:right, :padx=>2)
    TkLabel.new(f, :text=>'MOUSE-POINTER:').pack(:side=>:right)

    cmd = proc{|x, y|
      @l.text = '%02d:%02d' % coords_to_time(@c.canvasx(x), @c.canvasy(y))
    }
    @c.bind('Motion', cmd, '%x %y')
    @tag.bind('Motion', cmd, '%x %y')

    _create_hands
    _create_marks

    timer_proc = proc{
      t = Time.now
      @now.text = '%02d:%02d:%02d' % [t.hour, t.min, t.sec]
      set_hands(t.hour, t.min, t.sec)
    }

    timer_proc.call
    @timer = TkRTTimer.start(100, -1, timer_proc)
  end

  def _create_marks
    @mark_tag = TkcTag.new(@c)

    TkcLine.new(@c, 0, -0.90*@size, 0, -0.85*@size,
                :tags=>[@tag, @mark_tag],
                :width=>@mark_width, :fill=>@mark_color)
    TkcLine.new(@c, 0.90*@size, 0, 0.85*@size, 0,
                :tags=>[@tag, @mark_tag],
                :width=>@mark_width, :fill=>@mark_color)
    TkcLine.new(@c, 0, 0.90*@size, 0, 0.85*@size,
                :tags=>[@tag, @mark_tag],
                :width=>@mark_width, :fill=>@mark_color)
    TkcLine.new(@c, -0.90*@size, 0, -0.85*@size, 0,
                :tags=>[@tag, @mark_tag],
                :width=>@mark_width, :fill=>@mark_color)

    TkcText.new(@c, [0, -0.92*@size], :text=>0,
                :anchor=>'s', :fill=>@mark_color)
    TkcText.new(@c, [0.92*@size, 0], :text=>@clock.div(4),
                :anchor=>'w', :fill=>@mark_color)
    TkcText.new(@c, [0, 0.92*@size], :text=>@clock.div(2),
                :anchor=>'n', :fill=>@mark_color)
    TkcText.new(@c, [-0.92*@size, 0], :text=>@clock.div(4)*3,
                :anchor=>'e', :fill=>@mark_color)

    [30.0, 60.0].each{|angle|
      rad = Math::PI * angle / 180.0
      x_base = @size*Math::sin(rad)
      y_base = @size*Math::cos(rad)

      x1 = 0.90*x_base
      y1 = 0.90*y_base

      x2 = 0.85*x_base
      y2 = 0.85*y_base

      TkcLine.new(@c, x1, y1, x2, y2,
                  :tags=>[@tag, @mark_tag],
                  :width=>@mark_width, :fill=>@mark_color)
      TkcLine.new(@c, x1, -y1, x2, -y2,
                  :tags=>[@tag, @mark_tag],
                  :width=>@mark_width, :fill=>@mark_color)
      TkcLine.new(@c, -x1, y1, -x2, y2,
                  :tags=>[@tag, @mark_tag],
                  :width=>@mark_width, :fill=>@mark_color)
      TkcLine.new(@c, -x1, -y1, -x2, -y2,
                  :tags=>[@tag, @mark_tag],
                  :width=>@mark_width, :fill=>@mark_color)

      x3 = 0.92*x_base
      y3 = 0.92*y_base

      if @clock == 24
        dh = angle.to_i/15
      else # @clock == 12
        dh = angle.to_i/30
      end

      TkcText.new(@c, x3, -y3, :text=>dh,
                  :anchor=>'sw', :fill=>@mark_color)
      TkcText.new(@c, x3, y3, :text=>@clock.div(2)-dh,
                  :anchor=>'nw', :fill=>@mark_color)
      TkcText.new(@c, -x3, y3, :text=>@clock.div(2)+dh,
                  :anchor=>'ne', :fill=>@mark_color)
      TkcText.new(@c, -x3, -y3, :text=>@clock-dh,
                  :anchor=>'se', :fill=>@mark_color)
    }

    if @clock == 24
      [15.0, 45.0, 75.0].each{|angle|
        rad = Math::PI * angle / 180.0
        x_base = @size*Math::sin(rad)
        y_base = @size*Math::cos(rad)

        x1 = 0.90*x_base
        y1 = 0.90*y_base

        x2 = 0.875*x_base
        y2 = 0.875*y_base

        TkcLine.new(@c, x1, y1, x2, y2,
                    :tags=>[@tag, @mark_tag],
                    :width=>@mark_width, :fill=>@submark_color)
        TkcLine.new(@c, x1, -y1, x2, -y2,
                    :tags=>[@tag, @mark_tag],
                    :width=>@mark_width, :fill=>@submark_color)
        TkcLine.new(@c, -x1, y1, -x2, y2,
                    :tags=>[@tag, @mark_tag],
                    :width=>@mark_width, :fill=>@submark_color)
        TkcLine.new(@c, -x1, -y1, -x2, -y2,
                    :tags=>[@tag, @mark_tag],
                    :width=>@mark_width, :fill=>@submark_color)
      }
    end
  end

  def _create_hands
    hour_hand_len   = 0.55*@size
    minute_hand_len = 0.85*@size
    second_hand_len = 0.88*@size

    hour_hand_width   = 1.8*@cdot_size
    minute_hand_width = 1.0*@cdot_size
    second_hand_width = 1 # 0.4*@cdot_size

    @hour_hand_coords = [
      [0, -0.5*@cdot_size],
      [hour_hand_width, -0.5*@cdot_size-hour_hand_width],
      [hour_hand_width, -hour_hand_len+hour_hand_width],
      [0, -hour_hand_len],
      [-hour_hand_width, -hour_hand_len+hour_hand_width],
      [-hour_hand_width, -0.5*@cdot_size-hour_hand_width],
    ]
    @minute_hand_coords = [
      [0, -0.5*@cdot_size],
      [minute_hand_width, -0.5*@cdot_size - minute_hand_width],
      [minute_hand_width, -minute_hand_len+minute_hand_width],
      [0, -minute_hand_len],
      [-minute_hand_width, -minute_hand_len+minute_hand_width],
      [-minute_hand_width, -0.5*@cdot_size-minute_hand_width],
    ]
    @second_hand_coords = [
      [0, -0.5*@cdot_size],
      [second_hand_width, -0.5*@cdot_size - second_hand_width],
      [second_hand_width, -second_hand_len+second_hand_width],
      [0, -second_hand_len],
      [-second_hand_width, -second_hand_len+second_hand_width],
      [-second_hand_width, -0.5*@cdot_size-second_hand_width],
    ]

    @hour_hand = TkcPolygon.new(@c, @hour_hand_coords,
                                :tags=>[@tag, @hand_tag],
                                :outline=>@hour_hand_color,
                                :fill=>@hour_hand_color)

    @minute_hand = TkcPolygon.new(@c, @minute_hand_coords,
                                  :tags=>[@tag, @hand_tag],
                                  :outline=>@minute_hand_color,
                                  :fill=>@minute_hand_color)

    @second_hand = TkcPolygon.new(@c, @second_hand_coords,
                                  :tags=>[@tag, @hand_tag],
                                  :outline=>@second_hand_color,
                                  :fill=>@second_hand_color)

    @center_dot = TkcOval.new(@c,
                              [-@cdot_size, -@cdot_size],
                              [@cdot_size, @cdot_size],
                              :outline=>@cdot_color, :fill=>@cdot_color)
  end
  private :_create_hands

  def _raise_hands
    @hour_hand.raise
    @minute_hand.raise
    @second_hand.raise
    @center_dot.raise
  end
  private :_raise_hands

  def _raise_marks
    @mark_tag.raise
  end
  private :_raise_marks

  def set_hands(hh, mm, ss)
    ss_angle = Math::PI * ss / 30.0
    mm_angle = Math::PI * (mm + ss/60.0) / 30.0
    hh_angle = Math::PI * (hh + (mm + ss/60.0)/60.0) / (@clock.div(2))

    @second_hand.coords = @second_hand_coords.collect{|x, y|
      r = Math::hypot(y, x)
      a = Math::atan2(y, x) + ss_angle
      [Math::cos(a) * r, Math::sin(a) * r]
    }

    @minute_hand.coords = @minute_hand_coords.collect{|x, y|
      r = Math::hypot(y, x)
      a = Math::atan2(y, x) + mm_angle
      [Math::cos(a) * r, Math::sin(a) * r]
    }

    @hour_hand.coords = @hour_hand_coords.collect{|x, y|
      r = Math::hypot(y, x)
      a = Math::atan2(y, x) + hh_angle
      [Math::cos(a) * r, Math::sin(a) * r]
    }

    _raise_hands
    _raise_marks
  end

  def coords_to_time(x, y)
    return ((y < 0)? [0, 0]: [@clock.div(2), 0])  if x == 0
    if @clock == 24
      offset = (x<0&&y<0)? 1800.0: 360.0
      m_half = 720.0
    else # @clock == 12
      offset = (x<0&&y<0)? 900.0: 180.0
      m_half = 360.0
    end
    (offset + m_half*Math.atan2(y,x)/Math::PI).round.divmod(60)
  end

  def create_pie(hh, mm, span, color='red')
    if @clock == 24
      start  = 90.0 - (hh*60 + mm)/4.0  # 360.0*(hh*60+mm)/(24*60)
      extent = -span/4.0
    else # @clock == 12
      start  = 90.0 - (hh*60 + mm)/2.0  # 360.0*(hh*60+mm)/(12*60)
      extent = -span/2.0
    end

    pie = TkcArc.new(@c, @circle_coords, :tags=>[@tag],
                     :outline=>'black', 'fill'=>color,
                     :start=>start, :extent=>extent)
    _raise_hands
    _raise_marks
    pie
  end
end

sched = Clock.new
sched.create_pie(0,0, 60)            #  60 minutes from 00:00
sched.create_pie(6,30, 280, 'green') # 280 minutes from 06:30
sched.create_pie(15,20, 90, 'blue')  #  90 minutes from 15:20

Tk.mainloop
