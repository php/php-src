#!/usr/bin/env ruby

require 'tk'
require 'tkextlib/vu/dial'

#######################################

v_volume = TkVariable.new
v_speed  = TkVariable.new
v_dir    = TkVariable.new
v_rot    = TkVariable.new
v_linked = TkVariable.new
v_needle = TkVariable.new

volume = Tk::Vu::Dial.new(:label=>"Volume", :from=>-0.1, :to=>0.1,
                          :resolution=>0.001, :minortickinterval=>0.01,
                          :tickinterval=>0.1, :beginangle=>-20,
                          :endangle=>260, :variable=>v_volume)

speed = Tk::Vu::Dial.new(:label=>"Speed", :from=>2000, :to=>100,
                         :resolution=>10, :tickinterval=>100,
                         :minortickinterval=>0, :variable=>v_speed,
                         :showtags=>:label, :showvalue=>false)

speed.set_tag_constrain(100, 'Fast', 2000, 'Slow')

fwd  = Tk::Vu::Dial.new(:from=>-10.0, :to=>-20.0, :resolution=>0.1,
                        :tickinterval=>5.0, :minortickinterval=>1.0,
                        :variable=>v_dir)

rev  = Tk::Vu::Dial.new(:from=>-20.0, :to=>-10.0, :resolution=>0.1,
                        :tickinterval=>5.0, :minortickinterval=>1.0,
                        :variable=>v_dir)

small = Tk::Vu::Dial.new(:font=>"Helvetica -10", :from=>0, :to=>10,
                         :resolution=>0.05, :tickinterval=>2,
                         :minortickinterval=>0.5, :radius=>20,
                         :dialcolor=>'red2', :activebackground=>'red',
                         :variable=>v_rot)

large = Tk::Vu::Dial.new(:font=>"Helvetica -8", :from=>0, :to=>10,
                         :resolution=>0.05, :tickinterval=>1,
                         :minortickinterval=>0.25, :radius=>40,
                         :dialcolor=>'red2', :activebackground=>'red',
                         :variable=>v_rot)

turn = Tk::Vu::Dial.new(:needlecolor=>'red', :label=>"Linked",
                        :variable=>v_linked)

scale = TkScale.new(:label=>"Linked", :variable=>v_linked)

d1 = Tk::Vu::Dial.new(:resolution=>0.0001, :from=>-0.1, :to=>0.1,
                      :showvalue=>true, :minortickinterval=>0.01,
                      :tickinterval=>0.1, :radius=>30, :label=>"Dial",
                      :beginangle=>-20, :endangle=>260, :variable=>v_needle,
                      :relief=>:raised)

d2 = Tk::Vu::Dial.new(:resolution=>0.01, :from=>-0.1, :to=>0.1,
                      :showvalue=>true, :minortickinterval=>0.01,
                      :tickinterval=>0.1, :radius=>30, :label=>"Dial 2",
                      :beginangle=>-20, :endangle=>260, :variable=>v_needle,
                      :dialrelief=>:flat, :needlecolor=>'red',
                      :needletype=>:triangle, :relief=>:sunken)

d3 = Tk::Vu::Dial.new(:resolution=>0.001, :from=>-0.1, :to=>0.1,
                      :showvalue=>true, :minortickinterval=>0.01,
                      :tickinterval=>0.1, :radius=>30, :label=>"Dial 3",
                      :beginangle=>-20, :endangle=>260, :variable=>v_needle,
                      :dialrelief=>:flat, :needlecolor=>'blue',
                      :needletype=>:arc, :relief=>:ridge)

f_btns = TkFrame.new
f_sep  = TkFrame.new(:height=>2, :relief=>:sunken, :bd=>1)


v_volume.value = -0.1
v_speed.value = 500

update = TkTimer.new(proc{v_speed.numeric}, -1, proc{
                       if v_volume == volume[:to]
                         v_volume.numeric = volume[:from]
                       else
                         v_volume.numeric += volume[:resolution]
                       end
                     })

b_start = TkButton.new(:text=>"Start", :command=>proc{update.start})
b_stop  = TkButton.new(:text=>"Stop",  :command=>proc{update.stop})
b_exit  = TkButton.new(:text=>"Exit",  :command=>proc{exit})

Tk.grid(b_start, :in=>f_btns, :sticky=>:ew, :padx=>4, :pady=>4)
Tk.grid(b_stop,  :in=>f_btns, :sticky=>:ew, :padx=>4, :pady=>4)
f_btns.grid_columnconfigure(0, :weight=>1)

Tk.grid(f_btns, speed, volume, :sticky=>:news)
Tk.grid(f_sep,  '-',   '-',    :sticky=>:news)
Tk.grid(fwd,    rev,   d1,     :sticky=>:news)
Tk.grid(small,  large, d2,     :sticky=>:news)
Tk.grid(turn,   scale, d3,     :sticky=>:news)

Tk.grid(b_exit, '-',   '-',    :sticky=>:ew, :padx=>4, :pady=>4)

Tk.root.grid_columnconfigure(0, :weight=>1)
Tk.root.grid_columnconfigure(1, :weight=>1)
Tk.root.grid_columnconfigure(2, :weight=>1)

Tk.root.grid_rowconfigure(0, :weight=>1)
Tk.root.grid_rowconfigure(2, :weight=>1)
Tk.root.grid_rowconfigure(3, :weight=>1)
Tk.root.grid_rowconfigure(4, :weight=>1)


Tk.mainloop
