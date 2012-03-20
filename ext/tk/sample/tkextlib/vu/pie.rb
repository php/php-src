#!/usr/bin/env ruby

require 'tk'
require 'tkextlib/vu/pie'

pie = Tk::Vu::Pie.new(:label=>"My Revolving Budget"){
  itemconfigure('Welfare',   :value=>3.004)
  itemconfigure('Military',  :value=>7.006)
  itemconfigure('Transport', :value=>1.6, :explode=>15)
  itemconfigure('Parks',     :value=>0.9)
  itemconfigure('Schools',   :value=>2)
  itemconfigure('Debt',      :value=>4,   :explode=>10)

  configure(:angle=>10, :origin=>90, :shadow=>10)
}

spin = TkTimer.new(60, -1, proc{|obj|
  pie.configure(:origin=>pie[:origin] + 1)
})

f = TkFrame.new
fast_btn = TkButton.new(f, :text=>"Spin Faster", :command=>proc{spin.start})
slow_btn = TkButton.new(f, :text=>"Spin Slower", :command=>proc{spin.stop})
quit_btn = TkButton.new(f, :text=>"Exit", :command=>proc{exit})

Tk.grid(pie, :sticky=>:news)
Tk.grid(f, :sticky=>:ew)

Tk.pack(fast_btn, slow_btn, quit_btn,
        :in=>f, :side=>:left, :fill=>:both, :expand=>true, :padx=>6, :pady=>4)

Tk.root.grid_columnconfigure(0, :weight=>1)
Tk.root.grid_rowconfigure(0, :weight=>1)

priv = {
  :x=>0, :y=>0, :pie_in=>false, :angle=>pie[:angle], :origin=>pie[:origin]
}

pie.bind('ButtonPress-1', proc{|w, x, y|
             priv[:x] = x
             priv[:y] = y
             priv[:pie_in] = (w.winfo_width/1.8 > x)
             priv[:angle]  = w[:angle]
             priv[:origin] = w[:origin]
         }, '%W %x %y')

pie.bind('B1-Motion', proc{|w, x, y|
           if priv[:pie_in]
             w.configure(:angle=>priv[:angle] + (priv[:y] - y)/3,
                         :origin=>(priv[:origin] +
                                   ((w.winfo_height/2.2 > y)? -1: 1) *
                                   (priv[:x] - x)/3) % 360)
           end
         }, '%W %x %y')

Tk.mainloop
