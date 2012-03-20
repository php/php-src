#!/usr/bin/env ruby
##
## buttons.rb
##
## demonstrates the simulation of a button array
##
## ( based on 'buttons.tcl' included source archive of tktable extension )
##
require 'tk'
require 'tkextlib/tktable'

# create the table
tab  = TkVariable.new_hash
rows = 20
cols = 20

table = Tk::TkTable.new(:rows=>rows + 1, :cols=>cols + 1,
                        :variable=>tab, :titlerows=>1, :titlecols=>1,
                        :roworigin=>-1, :colorigin=>-1,
                        :colwidth=>4, :width=>8, :height=>8,
                        :cursor=>'top_left_arrow', :borderwidth=>2,
                        :flashmode=>false, :state=>:disabled)

sx = table.xscrollbar(TkScrollbar.new)
sy = table.yscrollbar(TkScrollbar.new)

Tk.grid(table, sy, :sticky=>:news)
Tk.grid(sx, :sticky=>:ew)

Tk.root.grid_columnconfig(0, :weight=>1)
Tk.root.grid_rowconfig(0, :weight=>1)

# set up tags for the various states of the buttons
table.tag_configure('OFF', :bg=>'red',    :relief=>:raised)
table.tag_configure('ON',  :bg=>'green',  :relief=>:sunken)
table.tag_configure('sel', :bg=>'gray75', :relief=>:flat)

# clean up if mouse leaves the widget
table.bind('Leave', proc{|w| w.selection_clear_all}, '%W')

# highlight the cell under the mouse
table.bind('Motion', proc{|w, x, y|
             Tk.callback_break if w.selection_include?(TkComm._at(x,y))
             w.selection_clear_all
             w.selection_set(TkComm._at(x,y))
             Tk.callback_break
             ## "break" prevents the call to tkTableCheckBorder
           }, '%W %x %y')

# mousebutton 1 toggles the value of the cell
# use of "selection includes" would work here
table.bind('1', proc{|w, x, y|
             #rc = w.curselection[0]
             rc = w.index(TkComm._at(x,y))
             if tab[rc] == 'ON'
               tab[rc] = 'OFF'
               w.tag_cell('OFF', rc)
             else
               tab[rc] = 'ON'
               w.tag_cell('ON', rc)
             end}, '%W %x %y')


# inititialize the array, titles, and celltags
0.step(rows){|i|
  tab[i,-1] = i
  0.step(cols){|j|
    if i == 0
      tab[-1,j] = j
    end
    tab[i,j] = "OFF"
    table.tag_cell('OFF', "#{i},#{j}")
  }
}

Tk.mainloop
