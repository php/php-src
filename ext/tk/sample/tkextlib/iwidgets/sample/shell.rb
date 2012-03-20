#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

sh = Tk::Iwidgets::Shell.new(:modality=>:application,
                             :padx=>20, :pady=>20, :title=>'Shell')

TkButton.new(:text=>'ACTIVATE', :padx=>7, :pady=>7,
             :command=>proc{puts sh.activate}).pack(:padx=>10, :pady=>10)

TkLabel.new(sh.child_site, :text=>'SHELL').pack
TkButton.new(sh.child_site, :text=>'YES',
             :command=>proc{sh.deactivate 'press YES'}).pack(:fill=>:x)
TkButton.new(sh.child_site, :text=>'NO',
             :command=>proc{sh.deactivate 'press NO'}).pack(:fill=>:x)

Tk.mainloop
