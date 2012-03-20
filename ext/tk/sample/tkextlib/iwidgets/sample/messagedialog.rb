#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

mainloop = Thread.new{Tk.mainloop}

#
# Standard question message dialog used for confirmation.
#
md = Tk::Iwidgets::Messagedialog.new(:title=>'Message Dialog',
                                     :text=>'Are you sure ? ',
                                     :bitmap=>'questhead', :modality=>:global)

md.buttonconfigure('OK', :text=>'Yes')
md.buttonconfigure('Cancel', :text=>'No')

if TkComm.bool(md.activate)
  md.text('Are you really sure ? ')
  if TkComm.bool(md.activate)
    puts 'Yes'
  else
    puts 'No'
  end
else
  puts 'No'
end

md.destroy

#
# Copyright notice with automatic deactivation.
#
bmp = '@' + File.join(File.dirname(File.expand_path(__FILE__)), '../catalog_demo/images/text.xbm')

cr = Tk::Iwidgets::Messagedialog.new(:title=>'Copyright',
                                     :bitmap=>bmp, :imagepos=>:n,
                                     :text=>"Copyright 200x XXX Corporation\nAll rights reserved")

cr.hide('Cancel')

cr.activate
Tk.after(7000, proc{cr.deactivate; Tk.root.destroy})

mainloop.join
