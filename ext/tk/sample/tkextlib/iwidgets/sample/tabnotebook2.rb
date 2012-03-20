#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

# Create the tabnotebook widget and pack it.
tn = Tk::Iwidgets::Tabnotebook.new(:width=>300, :height=>100)
tn.pack(:anchor=>:nw, :fill=>:both, :expand=>true,
        :side=>:top, :padx=>10, :pady=>0)

# Add two pages to the tabnotebook,
# labelled "Page One" and "Page Two"
tn.add(:label=>'Page One')
tn.add(:label=>'Page Two')

# Get the child site frames of these two pages.
page1CS = tn.child_site(0)
page2CS = tn.child_site('Page Two')

# Create buttons on each page of the tabnotebook.
TkButton.new(page1CS, :text=>'Button One').pack
TkButton.new(page2CS, :text=>'Button Two').pack

# Select the first page of the tabnotebook.
tn.select(0)

# Create the scrollbar
# and the notebook together, then pack the scrollbar
tn.xscrollbar(TkScrollbar.new).pack(:fill=>:x, :expand=>true, :padx=>10)

Tk.mainloop
