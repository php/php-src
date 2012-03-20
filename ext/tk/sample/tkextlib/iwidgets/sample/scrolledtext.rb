#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

st = Tk::Iwidgets::Scrolledtext.new(:hscrollmode=>:dynamic, :wrap=>:none,
                                    :labeltext=>'Password File')
st.pack(:expand=>true, :fill=>:both, :padx=>10, :pady=>10)

st.import('/etc/passwd')

Tk.mainloop
