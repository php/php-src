#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

#
# Non-editable Dropdown Combobox
#
cb1 = Tk::Iwidgets::Combobox.new(:labeltext=>'Month:',
                                 :selectioncommand=>proc{
                                   puts(cb1.get_curselection)
                                 },
                                 :editable=>false, :listheight=>185,
                                 :popupcursor=>'hand1')

cb1.insert_list('end', *%w(Jan Feb Mar Apr May June Jul Aug Sept Oct Nov Dec))


#
# Editable Dropdown Combobox
#
cb2 = Tk::Iwidgets::Combobox.new(:labeltext=>'Operating System:',
                                 :selectioncommand=>proc{
                                   puts(cb2.get_curselection)
                                 })

cb2.insert_list('end', *%w(Linux HP-UX SunOS Solaris Irix))
cb2.insert_entry('end', 'L')

cb1.pack(:padx=>10, :pady=>10, :fill=>:x)
cb2.pack(:padx=>10, :pady=>10, :fill=>:x)

Tk.mainloop
