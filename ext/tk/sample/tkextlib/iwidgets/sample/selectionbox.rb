#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

TkOption.add('*textBackground', 'white')

sb = Tk::Iwidgets::Selectionbox.new.pack(:padx=>10, :pady=>10,
                                         :fill=>:both, :expand=>true)

sb.insert_items('end', *['Hello', 'Out There', 'World'])

TkLabel.new(sb.child_site,
            :text=>'Child Site is Here').pack(:fill=>:x, :padx=>10, :pady=>10)

sb.insert_items(2, 'Cruel Cruel')

sb.selection_set(1)

Tk.mainloop
