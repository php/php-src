# -*- coding: utf-8 -*-
#
# widget demo 'load image' (called by 'widget')
#

# toplevel widget が存在すれば削除する
if defined?($image2_demo) && $image2_demo
  $image2_demo.destroy
  $image2_demo = nil
end

# demo 用の toplevel widget を生成
$image2_demo = TkToplevel.new {|w|
  title('Image Demonstration #2')
  iconname("Image2")
  positionWindow(w)
}

base_frame = TkFrame.new($image2_demo).pack(:fill=>:both, :expand=>true)

# label 生成
msg = TkLabel.new(base_frame) {
  font $font
  wraplength '4i'
  justify 'left'
  text "このデモではTkの photo image を使用して画像を見ることができます。最初にエントリ内ににディレクトリ名を入れて下さい。次に下のリストボックスにこのディレクトリをロードするため、リターンを押してください。その後、画像を選択するためにリストボックスの中のファイル名をダブルクリックして下さい。"
}
msg.pack('side'=>'top')

# frame 生成
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    #text '了解'
    text '閉じる'
    command proc{
      tmppath = $image2_demo
      $image2_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'コード参照'
    command proc{showCode 'image2'}
  }.pack('side'=>'left', 'expand'=>'yes')

}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# 変数生成
$dirName = TkVariable.new([$demo_dir,'..','images'].join(File::Separator))

# image 生成
$image2a = TkPhotoImage.new

# ファイル名入力部
TkLabel.new(base_frame, 'text'=>'ディレクトリ:')\
.pack('side'=>'top', 'anchor'=>'w')

image2_e = TkEntry.new(base_frame) {
  width 30
  textvariable $dirName
}.pack('side'=>'top', 'anchor'=>'w')

TkFrame.new(base_frame, 'height'=>'3m', 'width'=>20)\
.pack('side'=>'top', 'anchor'=>'w')

TkLabel.new(base_frame, 'text'=>'ファイル:')\
.pack('side'=>'top', 'anchor'=>'w')

TkFrame.new(base_frame){|w|
  s = TkScrollbar.new(w)
  l = TkListbox.new(w) {
    width 20
    height 10
    yscrollcommand proc{|first,last| s.set first,last}
  }
  s.command(proc{|*args| l.yview(*args)})
  l.pack('side'=>'left', 'expand'=>'yes', 'fill'=>'y')
  s.pack('side'=>'left', 'expand'=>'yes', 'fill'=>'y')
  #l.insert(0,'earth.gif', 'earthris.gif', 'mickey.gif', 'teapot.ppm')
  l.insert(0,'earth.gif', 'earthris.gif', 'teapot.ppm')
  l.bind('Double-1', proc{|x,y| loadImage $image2a,l,x,y}, '%x %y')

  image2_e.bind 'Return', proc{loadDir l}

}.pack('side'=>'top', 'anchor'=>'w')

# image 配置
[ TkFrame.new(base_frame, 'height'=>'3m', 'width'=>20),
  TkLabel.new(base_frame, 'text'=>'画像:'),
  # TkLabel.new(base_frame, 'image'=>$image2a)
  Tk::Label.new(base_frame, 'image'=>$image2a)
].each{|w| w.pack('side'=>'top', 'anchor'=>'w')}

# メソッド定義
def loadDir(w)
  w.delete(0,'end')
  Dir.glob([$dirName,'*'].join(File::Separator)).sort.each{|f|
    w.insert('end',File.basename(f))
  }
end

def loadImage(img,w,x,y)
  img.file([$dirName, w.get("@#{x},#{y}")].join(File::Separator))
end

