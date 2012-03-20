#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

mainloop = Thread.new{Tk.mainloop}

TkOption.add('*textBackground', 'white')

pd = Tk::Iwidgets::Promptdialog.new(:modality=>:global, :title=>'Password',
                                    :labeltext=>'Password:', :show=>'*')
pd.hide('Apply')

if TkComm.bool(pd.activate)
  puts "Password entered: #{pd.get}"
else
  puts "Password prompt cancelled"
end
