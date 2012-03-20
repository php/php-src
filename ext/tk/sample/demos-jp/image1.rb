# -*- coding: utf-8 -*-
#
# two image widgets demo (called by 'widget')
#

# toplevel widget が存在すれば削除する
if defined?($image1_demo) && $image1_demo
  $image1_demo.destroy
  $image1_demo = nil
end

# demo 用の toplevel widget を生成
$image1_demo = TkToplevel.new {|w|
  title('Image Demonstration #1')
  iconname("Image1")
  positionWindow(w)
}

base_frame = TkFrame.new($image1_demo).pack(:fill=>:both, :expand=>true)

# label 生成
msg = TkLabel.new(base_frame) {
  font $font
  wraplength '4i'
  justify 'left'
  text "このデモでは2つのラベル上に画像をそれぞれ表示しています。"
}
msg.pack('side'=>'top')

# frame 生成
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    #text '了解'
    text '閉じる'
    command proc{
      tmppath = $image1_demo
      $image1_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'コード参照'
    command proc{showCode 'image1'}
  }.pack('side'=>'left', 'expand'=>'yes')

}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# image 生成
image1a = \
TkPhotoImage.new('file'=>[$demo_dir,'..',
                          'images','earth.gif'].join(File::Separator))
image1b = \
TkPhotoImage.new('file'=>[$demo_dir,'..',
                          'images','earthris.gif'].join(File::Separator))

# label 生成
#[ TkLabel.new(base_frame, 'image'=>image1a, 'bd'=>1, 'relief'=>'sunken'),
#  TkLabel.new(base_frame, 'image'=>image1b, 'bd'=>1, 'relief'=>'sunken')
#].each{|w| w.pack('side'=>'top', 'padx'=>'.5m', 'pady'=>'.5m')}
[ Tk::Label.new(base_frame, 'image'=>image1a, 'bd'=>1, 'relief'=>'sunken'),
  Tk::Label.new(base_frame, 'image'=>image1b, 'bd'=>1, 'relief'=>'sunken')
].each{|w| w.pack('side'=>'top', 'padx'=>'.5m', 'pady'=>'.5m')}

