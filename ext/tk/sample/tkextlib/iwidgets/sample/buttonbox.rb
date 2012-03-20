#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

# sample 1
p bb1 = Tk::Iwidgets::Buttonbox.new
p bb1.add('Yes',   :text=>'Yes',   :command=>proc{puts 'Yes'})
p bb1.add('No',    :text=>'No',    :command=>proc{puts 'No'})
p bb1.add('Maybe', :text=>'Maybe', :command=>proc{puts 'Maybe'})
bb1.default('Yes')
bb1.pack(:expand=>true, :fill=>:both, :pady=>5)
print "\n"

# sample 2
p bb2 = Tk::Iwidgets::Buttonbox.new
p btn1 = bb2.add(:text=>'Yes',   :command=>proc{puts 'Yes'})
p btn2 = bb2.add(:text=>'No',    :command=>proc{puts 'No'})
p btn3 = bb2.add(:text=>'Maybe', :command=>proc{puts 'Maybe'})
bb2.default(btn1)
bb2.pack(:expand=>true, :fill=>:both, :pady=>5)

Tk.mainloop
