#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

mainloop = Thread.new{Tk.mainloop}

#
# Non-modal example
#
nmfsd = Tk::Iwidgets::Fileselectiondialog.new(:title=>'Non-Modal')
nmfsd.buttonconfigure('OK', :command=>proc{
                        puts "You selected #{nmfsd.get}"
                        nmfsd.deactivate
                      })
nmfsd.activate

#
# Modal example
#
mfsd = Tk::Iwidgets::Fileselectiondialog.new(:modality=>:application)
mfsd.center
if TkComm.bool(mfsd.activate)
  puts "You selected #{mfsd.get}"
else
  puts "You cancelled the dialog"
end

mainloop.join
