# -*- coding: utf-8 -*-
#
# form widget demo (called by 'widget')
#

# toplevel widget が存在すれば削除する
if defined?($form_demo) && $form_demo
  $form_demo.destroy
  $form_demo = nil
end

# demo 用の toplevel widget を生成
$form_demo = TkToplevel.new {|w|
  title("Form Demonstration")
  iconname("form")
  positionWindow(w)
}

base_frame = TkFrame.new($form_demo).pack(:fill=>:both, :expand=>true)

# label 生成
msg = TkLabel.new(base_frame) {
  font $font
  wraplength '4i'
  justify 'left'
  text "このウィンドウは簡単なフォーム入力用になっていて、さまざまなエントリに入力ができます。タブでエントリの切替えができます。"
}
msg.pack('side'=>'top', 'fill'=>'x')

# frame 生成
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    #text '了解'
    text '閉じる'
    command proc{
      tmppath = $form_demo
      $form_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'コード参照'
    command proc{showCode 'form'}
  }.pack('side'=>'left', 'expand'=>'yes')
}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# entry 生成
form_data = []
(1..5).each{|i|
  f = TkFrame.new(base_frame, 'bd'=>2)
  e = TkEntry.new(f, 'relief'=>'sunken', 'width'=>40)
  l = TkLabel.new(f)
  e.pack('side'=>'right')
  l.pack('side'=>'left')
  form_data[i] = {'frame'=>f, 'entry'=>e, 'label'=>l}
}

# 文字列設定
form_data[1]['label'].text('名前:')
form_data[2]['label'].text('住所:')
form_data[5]['label'].text('電話:')

# pack
(1..5).each{|i| form_data[i]['frame'].pack('side'=>'top', 'fill'=>'x')}

