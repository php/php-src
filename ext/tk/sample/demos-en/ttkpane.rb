# ttkpane.rb --
#
# This demonstration script creates a Ttk pane with some content.
#
# based on "Id: ttkpane.tcl,v 1.3 2007/12/13 15:27:07 dgp Exp"

if defined?($ttkpane_demo) && $ttkpane_demo
  $ttkpane_demo.destroy
  $ttkpane_demo = nil
end

$ttkpane_demo = TkToplevel.new {|w|
  title("Themed Nested Panes")
  iconname("ttkpane")
  positionWindow(w)
}

base_frame = TkFrame.new($ttkpane_demo).pack(:fill=>:both, :expand=>true)

Ttk::Label.new(base_frame, :font=>$font, :wraplength=>'4i', :justify=>:left,
               :text=><<EOL).pack(:side=>:top, :fill=>:x)
This demonstration shows off a nested set of themed paned windows. \
Their sizes can be changed by grabbing the area \
between each contained pane and dragging the divider.
EOL

Ttk::Separator.new(base_frame).pack(:side=>:top, :fill=>:x)

## See Code / Dismiss
Ttk::Frame.new(base_frame) {|frame|
  sep = Ttk::Separator.new(frame)
  Tk.grid(sep, :columnspan=>4, :row=>0, :sticky=>'ew', :pady=>2)
  TkGrid('x',
         Ttk::Button.new(frame, :text=>'See Code',
                         :image=>$image['view'], :compound=>:left,
                         :command=>proc{showCode 'ttkpane'}),
         Ttk::Button.new(frame, :text=>'Dismiss',
                         :image=>$image['delete'], :compound=>:left,
                         :command=>proc{
                           $ttkpane_demo.destroy
                           $ttkpane_demo = nil
                         }),
         :padx=>4, :pady=>4)
  grid_columnconfigure(0, :weight=>1)
  pack(:side=>:bottom, :fill=>:x)
}

frame = Ttk::Frame.new(base_frame).pack(:fill=>:both, :expand=>true)

outer = Ttk::Panedwindow.new(frame, :orient=>:horizontal)
outer.add(in_left = Ttk::Panedwindow.new(outer, :orient=>:vertical))
outer.add(in_right = Ttk::Panedwindow.new(outer, :orient=>:vertical))
in_left.add(left_top = Ttk::Labelframe.new(in_left, :text=>'Button'))
in_left.add(left_bot = Ttk::Labelframe.new(in_left, :text=>'Clocks'))
in_right.add(right_top = Ttk::Labelframe.new(in_right, :text=>'Progress'))
in_right.add(right_bot = Ttk::Labelframe.new(in_right, :text=>'Text'))
if Tk.windowingsystem == 'aqua'
  [left_top, left_bot, right_top, right_bot].each{|w| w.padding(3) }
end

# Fill the button pane
Ttk::Button.new(left_top, :text=>'Press Me',
                :command=>proc{
                  Tk.messageBox(:type=>'ok', :icon=>'info', :message=>'Ouch!',
                                :detail=>'That hurt...', :parent=>base_frame,
                                :title=>'Button Pressed')
                }).pack(:padx=>2, :pady=>5)


zones_list = [
  [':Europe/Berlin'],
  [':America/Argentina/Buenos_Aires', ':America/Buenos_Aires'],
  [':Africa/Johannesburg'],
  [':Europe/London'],
  [':America/Los_Angeles'],
  [':Europe/Moscow'],
  [':America/New_York'],
  [':Asia/Singapore'],
  [':Australia/Sydney'],
  [':Asia/Tokyo'],
]

zones = []

# Check tzinfo support
if $tk_major_ver > 8 || ($tk_major_ver == 8 && $tk_minor_ver >= 5)
  tzinfo = :tcl

  # Force a pre-load of all the timezones needed; otherwise can end up
  # poor-looking synch problems!
  zones_list.each{|list|
    list.each{|zone|
      begin
        Tk.tk_call('clock', 'format', '0', '-timezone', zone)
      rescue RuntimeError
        # ignore
      else
        zones << [zone, zone[%r<[^/:]+$>].tr('_', ' ')]
        break
      end
    }
  }

else
  begin
    require 'tzinfo'
    tzinfo = :tzinfo
  rescue Exception
    begin
      require 'tzfile'
      tzinfo = :tzfile
    rescue Exception
      tzinfo = nil
    end
  end

  case tzinfo
  when :tzinfo
    zones_list.each{|list|
      list.each{|zone|
        begin
          tz = TZInfo::Timezone.get(zone[%r<[^:]+$>])
        rescue Exception
          # ignore
        else
          zones << [tz, zone[%r<[^/:]+$>].tr('_', ' ')]
          break
        end
      }
    }

  when :tzfile
    zones_list.each{|list|
      list.each{|zone|
        begin
          tz = TZFile.create(zone[%r<[^:]+$>])
        rescue Exception
          # ignore
        else
          zones << [tz, zone[%r<[^/:]+$>].tr('_', ' ')]
          break
        end
      }
    }

  else
    [ -7, -4, -2, -1, 0, +1, +3, +8, +9, +10 ].each{|zone|
      zones << [zone, 'UTC%+03d00' % zone]
    }
  end
end

time = TkVariable.new_hash

case tzinfo
when :tcl
  update_proc = proc{|now, tz, label|
    time[label] = Tk.tk_call('clock', 'format', now.tv_sec,
                             '-timezone', tz, '-format', '%T')
  }
when :tzinfo
  update_proc = proc{|now, tz, label|
    time[label] = tz.utc_to_local(now).strftime('%H:%M:%S')
  }
when :tzfile
  update_proc = proc{|now, tz, label|
    time[label] = tz.at(now.tv_sec).strftime('%H:%M:%S')
  }
else
  update_proc = proc{|now, tz, label|
    time[label] = (now + (tz * 3600)).strftime('%H:%M:%S')
  }
end

# Fill the clocks pane
zones.each_with_index{|(zone, label), idx|
  Ttk::Separator.new(left_bot).pack(:fill=>:x) if idx > 0
  Ttk::Label.new(left_bot, :text=>label, :anchor=>'w').pack(:fill=>:x)
  Ttk::Label.new(left_bot, :textvariable=>time.ref(label),
                 :anchor=>'w').pack(:fill=>:x)
}

# Timer start
every = proc{
  now = Time.now.utc
  zones.each{|zone, label| update_proc.call(now, zone, label) }
}
TkRTTimer.new(1000, -1, every).start(0, every)

# Fill the progress pane
Ttk::Progressbar.new(right_top, :mode=>:indeterminate).pack(:fill=>:both, :expand=>true).start

# Fill the text pane
if Tk.windowingsystem != 'aqua'
  # The trick with the ttk::frame makes the text widget look like it fits with
  # the current Ttk theme despite not being a themed widget itself. It is done
  # by styling the frame like an entry, turning off the border in the text
  # widget, and putting the text widget in the frame with enough space to allow
  # the surrounding border to show through (2 pixels seems to be enough).
  f = Ttk::Frame.new(right_bot, :style=>Ttk::Entry)
  txt = TkText.new(frame, :wrap=>:word, :width=>30, :borderwidth=>0)
  txt.pack(:fill=>:both, :expand=>true, :in=>f, :pady=>2, :padx=>2)
  scr = txt.yscrollbar(Ttk::Scrollbar.new(frame))
  scr.pack(:side=>:right, :fill=>:y, :in=>right_bot)
  f.pack(:fill=>:both, :expand=>true)
  outer.pack(:fill=>:both, :expand=>true)
else
  txt = TkText.new(frame, :wrap=>:word, :width=>30, :borderwidth=>0)
  scr = txt.yscrollbar(TkScrollbar.new(frame))
  scr.pack(:side=>:right, :fill=>:y, :in=>right_bot)
  txt.pack(:fill=>:both, :expand=>true, :in=>right_bot)
  outer.pack(:fill=>:both, :expand=>true, :padx=>10, :pady=>[6, 10])
end
