#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/blt'

file = File.join(File.dirname(File.expand_path(__FILE__)),
                 'images', 'sample.gif')
if File.exist?(file)
  src = TkPhotoImage.new(:file=>file)
else
  fail RuntimeError, 'no image file'
end

width = src.width
height = src.height

TkOption.add('*Label.font', '*helvetica*10*')
TkOption.add('*Label.background', 'white')

l_img0 = TkLabel.new(:image=>src)
l_hdr0 = TkLabel.new(:text=>"#{width} x #{height}")
l_ftr0 = TkLabel.new(:text=>'100%')
Tk.root.background('white')

(2..10).each{|i|
  iw = width/i
  ih = height/i
  r = '%6g'%(100.0/i)
  dst = TkPhotoImage.new(:width=>iw, :height=>ih)
  Tk::BLT::Winop.image_resample(src, dst, :sinc)
  l_hdr = TkLabel.new(:text=>"#{iw} x #{ih}")
  l_ftr = TkLabel.new(:text=>"#{r}%")
  l_img = TkLabel.new(:image=>dst)
  Tk::BLT::Table.add(Tk.root,
                     [0,i], l_hdr,
                     [1,i], l_img,
                     [2,i], l_ftr)
  Tk.update
}

Tk.mainloop
