#!/usr/bin/env ruby
##
## debug.rb
##
## This demo uses most features of the table widget
##
## ( based on 'debug.tcl' included source archive of tktable extension )
##
require 'tk'
require 'tkextlib/tktable'

# create the table
ary  = TkVariable.new_hash
rows = 25
cols = 20

# fill table variable
((-(rows))..rows).each{|x|
  ((-(cols))..cols).each{|y|
    ary[x,y] = "r#{x},c#{y}"
  }
}

lbl = TkLabel.new(:text=>"TkTable v2 Example")

table = Tk::TkTable.new(:rows=>rows, :cols=>cols, :variable=>ary,
                        :width=>6, :height=>6,
                        :titlerows=>1, :titlecols=>2,
                        :roworigin=>-5, :colorigin=>-2,
                        :coltagcommand=>proc{|col|
                          col = Integer(col)
                          (col>0 && col%2 == 1)? 'OddCol': ''
                        },
                        :selectmode=>:extended, :flashmode=>true,
                        :rowstretch=>:unset, :colstretch=>:unset,
                        :selecttitles=>false, :drawmode=>:single)

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
table.tag_configure('title',  :bg=>'red',   :fg=>'green', :relief=>:sunken)
table.tag_configure('dis',    :state=>:disabled)

first = table[:colorigin]
%w(n s e w nw ne sw se c).each_with_index{|anchor, idx|
  table.tag_configure(anchor, :anchor=>anchor)
  table.tag_row(anchor, idx)
  table.set([idx,first], anchor)
}
courier = TkFont.new(:family=>'Courier', :size=>10)
table.tag_configure('s', :font=>courier, :justify=>:center)

logo = TkPhotoImage.new(:file=>File.join(File.dirname(File.expand_path(__FILE__)), 'tcllogo.gif'))
table.tag_configure('logo', :image=>logo, :showtext=>true)
table.tag_cell('logo', [1,2], [2,3], [4,1])
table.tag_cell('dis', [2,1], [1,-1], [3,0])
table.set_width([-2,8], [-1,9], [0, 12], [4, 14])

table.set([1,1], "multi-line\ntext\nmight be\ninteresting",
          [3,2], "more\nmulti-line\nplaying\n",
          [2,2], "null\0byte")

# This is in the row span
l = TkLabel.new(table, :text=>'Window s', :bg=>'yellow')
table.window_configure([6,0], :sticky=>:s, :window=>l)

# This is in the row titles
l = TkLabel.new(table, :text=>'Window ne', :bg=>'yellow')
table.window_configure([4,-1], :sticky=>:ne, :window=>l)

# This will get swallowed by a span
l = TkLabel.new(table, :text=>'Window ew', :bg=>'yellow')
table.window_configure([5,3], :sticky=>:ew, :window=>l)

# This is in the col titles
l = TkLabel.new(table, :text=>'Window news', :bg=>'yellow')
table.window_configure([-5,1], :sticky=>:news, :window=>l)

l = TkLabel.new(table.winfo_parent, :text=>'Sibling l', :bg=>'orange')
table.window_configure([5,1], :sticky=>:news, :window=>l)

if table.span_list.empty?
  table.set_spans([-1,-2], [0,3], [1,2], [0,5], [3,2], [2,2], [6,0], [4,0])
end

puts "Table is #{table.path} with array #{(table['variable'])}"

# table.postscript(:file=>'out.ps', :first=>:origin, :last=>[2,2])

Tk.mainloop
