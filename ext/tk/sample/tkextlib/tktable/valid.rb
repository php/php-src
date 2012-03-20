#!/usr/bin/env ruby
##
## valid.rb
##
## This demos shows the use of the validation mechanism of the table
## and uses the table's cache (no -command or -variable)
##
## ( based on 'valid.tcl' included source archive of tktable extension )
##
require 'tk'
require 'tkextlib/tktable'

rows = 10
cols = 10

def colorize(num)
  num = Integer(num)
  return 'colored' if (num > 0 && num % 2 == 1)
end

def fill_headers(w, r=10, c=10)
  (1..(r-1)).each{|i| w.set([i,0], i.to_s)}

  (1..(c-1)).each{|j|
    if j % 3 == 1
      w.set([0,j], 'AlphaNum')
    elsif j % 2 == 1
      w.set([0,j], 'Alpha')
    elsif j != 0
      w.set([0,j], 'Real')
    end
  }
end

def validate_proc(c, val)
  if c % 3 == 1
    # AlphaNum
    regexp = /^[A-Za-z0-9 ]*$/
  elsif c % 2 == 1
    # Alpha
    regexp = /^[A-Za-z ]*$/
  elsif c != 0
    # 'Real'
    regexp = /^[-+]?[0-9]*\.?[0-9]*([0-9]\.?e[-+]?[0-9]*)?$/
  end
  if val =~ regexp
    return true
  else
    Tk.bell
    return false
  end
end

lbl = TkLabel.new(:text=>"TkTable v1 Validated Table Example")

table = Tk::TkTable.new(:rows=>rows, :cols=>cols, :cache=>1,
                        :width=>5, :height=>5, :titlerows=>1, :titlecols=>1,
                        :coltagcommand=>proc{|n| colorize(n)},
                        :flashmode=>true, :selectmode=>:extended,
                        :colstretch=>:unset, :rowstretch=>:unset,
                        :validate=>true,
                        :validatecommand=>proc{|e|
                          unless e.widget.tag_include?('title', e.index)
                            validate_proc(e.column, e.new_value)
                          end } )

fill_headers(table)

table.tag_configure('colored', :bg=>'lightblue')
table.tag_configure('title',   :fg=>'red')
table.set_width(0,3)

sx = table.xscrollbar(TkScrollbar.new)
sy = table.yscrollbar(TkScrollbar.new)

btn = TkButton.new(:text=>'Exit', :command=>proc{exit})

Tk.grid(lbl, '-', :sticky=>:ew)
Tk.grid(table, sy, :sticky=>:news)
Tk.grid(sx, :sticky=>:ew)
Tk.grid(btn, '-', :sticky=>:ew)

Tk.root.grid_columnconfig(0, :weight=>1)
Tk.root.grid_rowconfig(1, :weight=>1)

puts "Table is #{table.path}"

Tk.mainloop
