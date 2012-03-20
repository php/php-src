#!/usr/bin/env ruby
#
#   countdown timer
#     usage: cd_timer min [, min ... ]
#            ( e.g. cd_timer 0.5 1 3 5 10 )
#
require 'tk'

if ARGV.empty?
  $stderr.puts 'Error:: No time arguments for counting down'
  exit(1)
end

width = 10

TkButton.new(:text=>'exit',
             :command=>proc{exit}).pack(:side=>:bottom, :fill=>:x)

b = TkButton.new(:text=>'start').pack(:side=>:top, :fill=>:x)

f = TkFrame.new(:relief=>:ridge, :borderwidth=>2).pack(:fill=>:x)
TkLabel.new(f, :relief=>:flat, :pady=>3,
            :background=>'black', :foreground=>'white',
            :text=>'  elapsed: ').pack(:fill=>:x, :side=>:left, :expand=>true)
now = TkLabel.new(f, :width=>width, :relief=>:flat, :pady=>3, :anchor=>:w,
                  :background=>'black', :foreground=>'white',
                  :text=>'%4d:%02d.00' % [0, 0]).pack(:side=>:right)

timers = [ TkRTTimer.new(10){|tm|
    t = (tm.return_value || 0) + 1
    s, u = t.divmod(100)
    m, s = s.divmod(60)
    now.text('%4d:%02d.%02d' % [m, s, u])
    t
  }.set_start_proc(0, proc{
                     now.text('%4d:%02d.00' % [0,0])
                     now.foreground('white')
                     0
                   })
]

ARGV.collect{|arg| (Float(arg) * 60).to_i}.sort.each_with_index{|time, idx|
  f = TkFrame.new(:relief=>:ridge, :borderwidth=>2).pack(:fill=>:x)
  TkLabel.new(f, :relief=>:flat, :pady=>3,
              :text=>'  %4d:%02d  --> ' % (time.divmod(60))).pack(:side=>:left)
  l = TkLabel.new(f, :width=>width, :relief=>:flat, :pady=>3, :anchor=>:w,
                  :text=>'%4d:%02d' % (time.divmod(60))).pack(:side=>:right)
  timers << TkRTTimer.new(1000){|tm|
    t = (tm.return_value || time) - 1
    if t < 0
      l.text('%4d:%02d' % ((-t).divmod(60)))
    else
      l.text('%4d:%02d' % (t.divmod(60)))
    end
    if t.zero?
      l.foreground('red')
      idx.times{Tk.bell}
    end
    t
  }.set_start_proc(0, proc{
                     l.text('%4d:%02d' % (time.divmod(60)))
                     l.foreground('black')
                     time
                   })
}

mode = :start
b.command(proc{
            if mode == :start
              timers.each{|timer| timer.restart}
              b.text('reset')
              mode = :reset
            else
              timers.each{|timer| timer.stop.reset}
              b.text('start')
              mode = :start
            end
          })

Tk.mainloop

