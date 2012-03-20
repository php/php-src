#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

Tk::Iwidgets::Extbutton.new(:text=>'Bitmap example', :bitmap=>'info',
                            :background=>'bisque',  :activeforeground=>'red',
                            :bitmapforeground=>'blue', :defaultring=>true,
                            :command=>proc{
                              puts "Bisque is beautiful"
                            }).pack(:expand=>true)

#img = TkPhotoImage.new(:file=>File.join(File.dirname(File.expand_path(__FILE__)), '../../../images/earthris.gif'))
img = TkPhotoImage.new(:file=>File.join(File.dirname(File.expand_path(__FILE__)), '../catalog_demo/images/clear.gif'))

Tk::Iwidgets::Extbutton.new(:text=>'Image example', :relief=>:ridge,
                            :image=>img, :imagepos=>:e, :font=>'9x15bold',
                            :activebackground=>'lightyellow',
                            :background=>'lightgreen').pack(:expand=>true)

Tk.mainloop
