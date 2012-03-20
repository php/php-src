# -*- coding: utf-8 -*-
#
# a dialog box with a local grab (called by 'widget')
#
class TkDialog_Demo1 < TkDialog
  ###############
  private
  ###############
  def title
    "Dialog with local grab"
  end

  def message
    'モーダルダイアログボックスです。Tk の "grab" コマンドを使用してダイアログボックスで「ローカルグラブ」しています。下のいずれかのボタンを実行することによって、このダイアログに答えるまで、このグラブによってアプリケーションの他のウィンドウでは、ポインタ関係のイベントを受け取ることができなくなっています。'
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

ret =  TkDialog_Demo1.new('message_config'=>{'wraplength'=>'4i'}).value
case ret
when 0
  print "あなたは「了解」を押しましたね。\n"
when 1
  print "あなたは「キャンセル」を押しましたね。\n"
when 2
  showCode 'dialog1'
end
