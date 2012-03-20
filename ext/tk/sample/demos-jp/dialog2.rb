# -*- coding: utf-8 -*-
#
# a dialog box with a global grab (called by 'widget')
#
class TkDialog_Demo2 < TkDialog
  ###############
  private
  ###############
  def title
    "Dialog with global grab"
  end

  def message
    'このダイアログボックスはグローバルグラブを使用しています。下のボタンを実行するまで、ディスプレイ上のいかなるものとも対話できません。グローバルグラブを使用することは、まず良い考えではありません。どうしても必要になるまで使おうと思わないで下さい。'
  end

  def bitmap
    'info'
  end

  def default_button
    0
  end

  def buttons
#    "了解 キャンセル コード参照"
    ["了解", "キャンセル", "コード参照"]
  end
end

ret =  TkDialog_Demo2.new('message_config'=>{'wraplength'=>'4i'},
                          'prev_command'=>proc{|dialog|
                            Tk.after 100, proc{dialog.grab('global')}
                          }).value
case ret
when 0
  print "あなたは「了解」を押しましたね。\n"
when 1
  print "あなたは「キャンセル」を押しましたね。\n"
when 2
  showCode 'dialog2'
end

