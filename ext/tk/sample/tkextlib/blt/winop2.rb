#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/blt'

file = File.join(File.dirname(File.expand_path(__FILE__)),
                 'images', 'qv100.t.gif')
if File.exist?(file)
  src = TkPhotoImage.new(:file=>file)
else
  fail RuntimeError, 'no image file'
end

width = src.width
height = src.height

TkOption.add('*Label.font', '*helvetica*10*')
TkOption.add('*Label.background', 'white')

[0, 90, 180, 270, 360, 45].each_with_index{|r, i|
  dest = TkPhotoImage.new
  Tk::BLT::Winop.image_rotate(src, dest, r)
  l_txt = TkLabel.new(:text=>"#{r} degrees")
  l_img = TkLabel.new(:image=>dest)
  Tk::BLT::Table.add(Tk.root, [0,i], l_img, [1,i], l_txt)
  Tk.update
}

Tk.mainloop
