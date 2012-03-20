#!/usr/bin/env ruby

require "tk"

TkMessage.new(:width=>360, :text=><<EOM).pack
This sample shows how to use a binary sequence between Ruby and Tk. \
This reads the image data from the file as the binary sequence.

To treat the difference of encodings between on Ruby and on Tk seamlessly, \
Ruby/Tk converts the encoding of string arguments automatically. \
I think it is comfortable for users on almost all situations. \
However, when treats a binary sequence, the convert process makes troubles.

Tk::BinaryString class (subclass of Tk::EncodedString class) is the class \
to avoid such troubles. Please see the source code of this sample. \
A Tk::BinaryString instance is used to create the image for the center button.
EOM

ImgFile=[File.dirname(__FILE__), 'images','tcllogo.gif'].join(File::Separator)

ph1 = TkPhotoImage.new(:file=>ImgFile)
p ph1.configinfo

b_str = Tk::BinaryString(IO.read(ImgFile))
p [b_str, b_str.encoding]

ph2 = TkPhotoImage.new(:data=>b_str)
p ph2.configinfo
p ph2.data(:grayscale=>true)

ph3 = TkPhotoImage.new(:palette=>256)
ph3.put(ph2.data)

ph4 = TkPhotoImage.new()
ph4.put(ph2.data(:grayscale=>true))

#p [b_str.encoding, b_str.rb_encoding]

f = TkFrame.new.pack
TkButton.new(:parent=>f, :image=>ph1, :command=>proc{exit}).pack(:side=>:left)
TkButton.new(:parent=>f, :image=>ph2, :command=>proc{exit}).pack(:side=>:left)
TkButton.new(:parent=>f, :image=>ph3, :command=>proc{exit}).pack(:side=>:left)
TkButton.new(:parent=>f, :image=>ph4, :command=>proc{exit}).pack(:side=>:left)

Tk.mainloop
