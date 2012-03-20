#!/usr/bin/env ruby
##
## command.rb
##
## This demo shows the use of the table widget's -command options
##
## ( based on 'command.tcl' included source archive of tktable extension )
##
require 'tk'
require 'tkextlib/tktable'

# create the table
data = TkVariable.new_hash
rows = 10
cols = 10

# fill table variable
((-(rows))..rows).each{|x|
  ((-(cols))..cols).each{|y|
    data[x,y] = "#{x} x #{y}"
  }
}

lbl = TkLabel.new(:text=>"TkTable :command Example")
cur_var = TkVariable.new
current = TkLabel.new(:textvariable=>cur_var, :width=>5)
ent_var = TkVariable.new
entry = TkEntry.new(:textvariable=>ent_var)

table = Tk::TkTable.new(:rows=>rows, :cols=>cols,
                        :command=>[proc{|mode, cell, val|
                          if (mode == :w)
                            data[cell] = val
                          else
                            begin
                              data[cell]  # exist
                            rescue
                              ''          # not exist
                            end
                          end
                        }, '%i %C %s'],
                        :width=>6, :height=>6,
                        :titlerows=>1, :titlecols=>1,
                        :roworigin=>-1, :colorigin=>-1,
                        :rowstretchmode=>:last, :colstretchmode=>:last,
                        :rowtagcommand=>proc{|row|
                          row = Integer(row)
                          (row>0 && row%2 == 1)? 'OddRow': ''
                        },
                        :coltagcommand=>proc{|col|
                          col = Integer(col)
                          (col>0 && col%2 == 1)? 'OddCol': ''
                        },
                        :selectmode=>:extended, :flashmode=>true,
                        :rowstretch=>:unset, :colstretch=>:unset,
                        :browsecommand=>[proc{|w, s|
                          cur_var.value = s
                          ent_var.value = w.get(s)
                        }, '%W %S'],
                        :validate=>true,
                        :validatecommand=>proc{|e|
                          ent_var.value = e.new_value; true
                        })
=begin
                        :validatecommand=>[
                          proc{|s|
                            ent_var.value = s; true
                          }, '%S'])
=end

sx = table.xscrollbar(TkScrollbar.new)
sy = table.yscrollbar(TkScrollbar.new)

entry.bind('Return', proc{|w| table.curvalue = w.value}, '%W')

Tk.grid(lbl, '-', '-', :sticky=>:ew)
Tk.grid(current, entry, '-', :sticky=>:ew)
Tk.grid(table, '-', sy, :sticky=>:news)
Tk.grid(sx, '-', :sticky=>:ew)

Tk.root.grid_columnconfig(1, :weight=>1)
Tk.root.grid_rowconfig(2, :weight=>1)

table.tag_configure('OddRow', :bg=>'orange', :fg=>'purple')
table.tag_configure('OddCol', :bg=>'brown', :fg=>'pink')

puts "Table is #{table.path}"

Tk.mainloop
