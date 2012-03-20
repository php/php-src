#!/usr/bin/env ruby
##
## maxsize.rb
##
## This demo uses a really big table.  The big startup time is in
## filling the table's Tcl array var.
##
## ( based on 'maxsize.tcl' included source archive of tktable extension )
##
require 'tk'
require 'tkextlib/tktable'

ary  = TkVariable.new_hash
rows = 40000
cols = 10

# fill table variable
((-(rows))..rows).each{|x|
  ((-(cols))..cols).each{|y|
    ary[x,y] = "#{x},#{y}"
  }
}

lbl = TkLabel.new(:text=>"TkTable v2 Example")

table = Tk::TkTable.new(:rows=>rows, :cols=>cols, :variable=>ary,
                        :width=>6, :height=>8,
                        :titlerows=>1, :titlecols=>1,
                        :coltagcommand=>proc{|col|
                          col = Integer(col)
                          (col>0 && col%2 == 1)? 'OddCol': ''
                        },
                        :selectmode=>:extended,
                        :colstretch=>:unset, :rowstretch=>:unset,
                        :selecttitles=>false, :drawmode=>:slow)

sx = table.xscrollbar(TkScrollbar.new)
sy = table.yscrollbar(TkScrollbar.new)

btn = TkButton.new(:text=>'Exit', :command=>proc{exit})

Tk.grid(lbl, '-', :sticky=>:ew)
Tk.grid(table, sy, :sticky=>:news)
Tk.grid(sx, :sticky=>:ew)
Tk.grid(btn, :sticky=>:ew, :columnspan=>2)

Tk.root.grid_columnconfig(0, :weight=>1)
Tk.root.grid_rowconfig(1, :weight=>1)

table.tag_configure('OddCol', :bg=>'brown', :fg=>'pink')
table.tag_configure('title',  :bg=>'red',   :fg=>'blue', :relief=>:sunken)
table.tag_configure('dis',    :state=>:disabled)

first = table[:colorigin]
%w(n s e w nw ne sw se c).each_with_index{|anchor, idx|
  table.tag_configure(anchor, :anchor=>anchor)
  table.tag_row(anchor, idx)
  table.set([idx,first], anchor)
}
courier = TkFont.new(:family=>'Courier', :size=>10)
table.tag_configure('s', :font=>courier, :justify=>:center)

table.set_width([-2, 8], [-1, 9], [0, 12], [4, 14])

puts "Table is #{table.path} with array #{(table['variable'])}"

Tk.mainloop
