#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

# Create a listbox with two items (one and two)
l = TkListbox.new(:selectmode=>:single, :exportselection=>false).pack
l.insert('end', 'one')
l.insert('end', 'two')
l.selection_set(0)

# Define a proc that knows how to select an item
# from a list given an index from the tabset -command callback.
selectItem = proc{|item|
  l.selection_clear(l.curselection)
  l.selection_set(item)
  l.see(item)
}

# Create a tabset, set its -command to call selectItem
# Add two labels to the tabset (one and two).
ts = Tk::Iwidgets::Tabset.new(:command=>selectItem)
ts.add(:label=>1)
ts.add(:label=>2)
ts.select(0)
ts.pack(:fill=>:x, :expand=>true)

# Define a proc that knows how to select a tab
# given a y pixel coordinate from the list..
selectTab = proc{|y| ts.select(l.nearest(y)) }

# bind button 1 press to the selectTab procedure.
l.bind('ButtonPress-1', proc{|y| selectTab.call(y) }, '%y')

Tk.mainloop
