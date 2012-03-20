#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

Tk::Iwidgets::Pushbutton.new(:text=>'Hello',
                             :command=>proc{puts 'Hello World'},
                             :defaultring=>true).pack(:padx=>10, :pady=>10)

Tk.mainloop
