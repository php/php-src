#!/usr/bin/env ruby

require 'tk'
require 'tkextlib/vu'

#######################################

puts "Show off barchart and dial widgets"

speed = TkVariable.new(0)

dial = Tk::Vu::Dial.new(:resolution=>0.001, :from=>-0.1, :to=>0.1,
                        :showvalue=>true, :minortickinterval=>0.01,
                        :tickinterval=>0.1, :radius=>50, :label=>"Dial",
                        :beginangle=>-20, :endangle=>260, :dialcolor=>'red3',
                        :active=>'red2', :variable=>speed)

bar = Tk::Vu::Bargraph.new(:from=>0, :to=>100, :relief=>:groove,
                           :border=>2, :label=>"Bar Chart")

#######################################

green   = 25
blue    = 50
purple  = 75
current = 50

def rand_bool

end

update = TkTimer.new(200, -1, proc{
                       if (rand() - 0.5 + speed.numeric * 3) > 0
                         current += 1
                       else
                         current -= 1
                       end
                       bar.set(current)
                       if current < green
                         current = 100 if current <= 0
                         bar[:barcolor] = 'green'
                       elsif current < blue
                         bar[:barcolor] = 'blue'
                       elsif current < purple
                         bar[:barcolor] = 'purple'
                       else
                         bar[:barcolor] = 'red'
                         current = 0 if current >= 100
                       end
                     })

#######################################

gobar = TkButton.new(:text=>"Start", :command=>proc{update.start})
nobar = TkButton.new(:text=>"Stop",  :command=>proc{update.stop})
quit  = TkButton.new(:text=>"Exit",  :command=>proc{exit})

Tk.grid('x', gobar, :sticky=>:ew, :padx=>4, :pady=>4)
Tk.grid(dial, bar, :sticky=>:news)
Tk.grid('x', nobar, :sticky=>:ew, :padx=>4, :pady=>4)
Tk.grid(quit, '-', '-', :sticky=>:ew, :padx=>4, :pady=>4)
Tk.root.grid_columnconfigure(2, :weight=>1)
Tk.root.grid_rowconfigure(1, :weight=>1)

#######################################

Tk.mainloop
