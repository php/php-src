# -*- coding: utf-8 -*-
#
# iconic button widget demo (called by 'widget')
#

# toplevel widget が存在すれば削除する
if defined?($icon_demo) && $icon_demo
  $icon_demo.destroy
  $icon_demo = nil
end

# demo 用の toplevel widget を生成
$icon_demo = TkToplevel.new {|w|
  title("Iconic Button Demonstration")
  iconname("icon")
  positionWindow(w)
}

base_frame = TkFrame.new($icon_demo).pack(:fill=>:both, :expand=>true)

# label 生成
msg = TkLabel.new(base_frame) {
  font $font
  wraplength '5i'
  justify 'left'
  text "このウィンドウにはラジオボタンとチェックボタン上にビットマップや画像を表示する 3 つの方法を示しています。左にあるのは2つのラジオボタンで、それぞれが、ビットマップと選択を示すインジケータでできています。中央にあるのは、選択済みかどうかによって異なる画像を表示するチェックボタンです。右側にあるのは選択済みかどうかによって背景色が変わるビットマップを表示するチェックボタンです。"
}
msg.pack('side'=>'top')

# frame 生成
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    #text '了解'
    text '閉じる'
    command proc{
      tmppath = $icon_demo
      $icon_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'コード参照'
    command proc{showCode 'icon'}
  }.pack('side'=>'left', 'expand'=>'yes')

}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# image 生成
flagup = \
TkBitmapImage.new('file'=>[$demo_dir,'..',
                           'images','flagup.xbm'].join(File::Separator),
                  'maskfile'=>\
                  [$demo_dir,'..','images','flagup.xbm'].join(File::Separator))
flagdown = \
TkBitmapImage.new('file'=>[$demo_dir,'..',
                           'images','flagdown.xbm'].join(File::Separator),
                  'maskfile'=>\
                  [$demo_dir,'..',
                    'images','flagdown.xbm'].join(File::Separator))

# 変数生成
letters = TkVariable.new

# frame 生成
TkFrame.new(base_frame, 'borderwidth'=>10){|w|
  TkFrame.new(w) {|f|
    # TkRadioButton.new(f){
    Tk::RadioButton.new(f){
      bitmap '@' + [$demo_dir,'..',
                     'images','letters.xbm'].join(File::Separator)
      variable letters
      value 'full'
    }.pack('side'=>'top', 'expand'=>'yes')

    # TkRadioButton.new(f){
    Tk::RadioButton.new(f){
      bitmap '@' + [$demo_dir,'..',
                     'images','noletter.xbm'].join(File::Separator)
      variable letters
      value 'empty'
    }.pack('side'=>'top', 'expand'=>'yes')

  }.pack('side'=>'left', 'expand'=>'yes', 'padx'=>'5m')

  # TkCheckButton.new(w) {
  Tk::CheckButton.new(w) {
    image flagdown
    selectimage flagup
    indicatoron 0
    selectcolor self['background']
  }.pack('side'=>'left', 'expand'=>'yes', 'padx'=>'5m')

  # TkCheckButton.new(w) {
  Tk::CheckButton.new(w) {
    bitmap '@' + [$demo_dir,'..',
                   'images','letters.xbm'].join(File::Separator)
    indicatoron 0
    selectcolor 'SeaGreen1'
  }.pack('side'=>'left', 'expand'=>'yes', 'padx'=>'5m')

}.pack('side'=>'top')

