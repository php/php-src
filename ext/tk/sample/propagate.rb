#!/usr/bin/env ruby
require 'tk'

TkLabel.new(:text=>"Please click the bottom frame").pack

f = TkFrame.new(:width=>400, :height=>100, :background=>'yellow',
                :relief=>'ridge', :borderwidth=>5).pack

# TkPack.propagate(f, false) # <== important!!
f.pack_propagate(false)      # <== important!!

list = (1..3).collect{|n|
  TkButton.new(f, :text=>"button#{'-X'*n}"){
    command proc{
      puts "button#{'-X'*n}"
      self.unpack
    }
  }
}

list.unshift(nil)

f.bind('1', proc{
         w = list.shift
         w.unpack if w
         list.push(w)
         list[0].pack(:expand=>true, :anchor=>:center) if list[0]
       })

Tk.mainloop
