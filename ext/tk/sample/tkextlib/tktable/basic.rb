#!/usr/bin/env ruby
##
## basic.rb
##
## This demo shows the basic use of the table widget
##
## ( based on 'basic.tcl' included source archive of tktable extension )
##
require 'tk'
require 'tkextlib/tktable'

ary  = TkVariable.new_hash
rows = 8
cols = 8

# fill table variable
((-(rows))..rows).each{|x|
  ((-(cols))..cols).each{|y|
    ary[x,y] = "r#{x},c#{y}"
  }
}

lbl = TkLabel.new(:text=>"TkTable v1 Example")

table = Tk::TkTable.new(:rows=>rows, :cols=>cols, :variable=>ary,
                        :width=>6, :height=>6,
                        :titlerows=>1, :titlecols=>2,
                        :roworigin=>-1, :colorigin=>-2,
                        :rowstretchmode=>:last, :colstretchmode=>:last,
                        :rowtagcommand=>proc{|row|
                          row = Integer(row)
                          (row>0 && row%2 == 1)? 'OddRow': ''
                        },
                        :coltagcommand=>proc{|col|
                          col = Integer(col)
                          (col>0 && col%2 == 1)? 'OddCol': ''
                        },
                        :selectmode=>:extended, :sparsearray=>false)

sx = table.xscrollbar(TkScrollbar.new)
sy = table.yscrollbar(TkScrollbar.new)

btn = TkButton.new(:text=>'Exit', :command=>proc{exit})

Tk.grid(lbl, '-', :sticky=>:ew)
Tk.grid(table, sy, :sticky=>:news)
Tk.grid(sx, :sticky=>:ew)
Tk.grid(btn, :sticky=>:ew, :columnspan=>2)

Tk.root.grid_columnconfig(0, :weight=>1)
Tk.root.grid_rowconfig(1, :weight=>1)

table.tag_configure('OddRow', :bg=>'orange', :fg=>'purple')
table.tag_configure('OddCol', :bg=>'brown', :fg=>'pink')

table.set_width([-2, 7], [-1, 7], [1, 5], [2, 8], [4, 14])

puts "Table is #{table.path} with array #{(table['variable'])}"

Tk.mainloop
