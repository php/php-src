# -*- coding: utf-8 -*-
#
# text (basic facilities) widget demo (called by 'widget')
#

# toplevel widget が存在すれば削除する
if defined?($text_demo) && $text_demo
  $text_demo.destroy
  $text_demo = nil
end

# demo 用の toplevel widget を生成
$text_demo = TkToplevel.new {|w|
  title("Text Demonstration - Basic Facilities")
  iconname("text")
  positionWindow(w)
}

base_frame = TkFrame.new($text_demo).pack(:fill=>:both, :expand=>true)

# version check
if ((Tk::TK_VERSION.split('.').collect{|n| n.to_i} <=> [8,4]) < 0)
  undo_support = false
else
  undo_support = true
end

# frame 生成
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    #text '了解'
    text '閉じる'
    command proc{
      tmppath = $text_demo
      $text_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'コード参照'
    command proc{showCode 'text'}
  }.pack('side'=>'left', 'expand'=>'yes')
}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# text 生成
TkText.new(base_frame){|t|
  # 生成
  relief 'sunken'
  bd 2
  setgrid 1
  height 30
  TkScrollbar.new(base_frame) {|s|
    pack('side'=>'right', 'fill'=>'y')
    command proc{|*args| t.yview(*args)}
    t.yscrollcommand proc{|first,last| s.set first,last}
  }
  pack('expand'=>'yes', 'fill'=>'both')

  # テキスト挿入
  insert('0.0', <<EOT)
このウィンドウはテキスト widget です。1行またはそれ以上のテキストを表
示・編集することができます。以下はテキスト widget でできる操作について
まとめたものです。

1. スクロール。スクロールバーでテキストの表示部分を動かすことができます。

2. スキャニング。テキストのウィンドウでマウスボタン2 (中ボタンを) を押
して上下にドラッグしてください。そうするとテキストが高速でドラッグされ、
内容をざっと眺めることができます。

3. テキストの挿入。マウスボタン1 (左ボタン) を押し、挿入カーソルをセッ
トしてからテキストを入力してください。入力したものが widget に入ります。

4. 選択。ある範囲の文字を選択するにはマウスボタン1 を押し、ドラッグし
てください。一度ボタンを離したら、シフトキーを押しながらボタン1 を押す
ことで選択範囲の調整ができます。これは選択範囲の最後をマウスカーソルに
最も近い位置にリセットし、ボタンを離す前にマウスをドラッグすることでさ
らに選択範囲を調整できます。ダブルクリックでワードを、またトリプルクリッ
クで行全体を選択することができます。

5. 消去と置換。テキストを消去するには、消去したい文字を選択してバック
スペースかデリートキーを入力してください。あるいは、新しいテキストを
入力すると選択されたテキストと置換されます。

6. 選択部分のコピー。選択部分をこのウィンドウの中のどこかにコピーする
には、まずコピーしたい所を選択(ここで、あるいは別のアプリケーションで)
し、ボタン 2 をクリックして、挿入カーソルの位置にコピーしてください。

7. 編集。テキスト widget は Emacs のキーバインドに加えて標準的なの Motif
の編集機能をサポートしています。バックスペースとコントロール-H は挿入
カーソルの左側の文字を削除します。デリートキーとコントロール-D は挿入
カーソルの右側の文字を削除します。Meta-バックスペースは挿入カーソルの
右側の単語を削除し、Meta-D は挿入カーソルの左側の単語を削除します。
コントロール-K は挿入カーソルから行末までを削除し、その位置に改行
しかなかった場合は、改行を削除します。#{
      if undo_support
        undo_text = "Control-z は最後に行った変更の取り消し(undo)を行い、"
        case $tk_platform['platform']
        when "unix", "macintosh"
          undo_text << "Control-Shift-z"
        else # 'windows'
          undo_text << "Control-y"
        end
        undo_text << "はundoした変更の再適用(redo)を行います。"
      else
        ""
      end
}


8. ウィンドウのリサイズ。この widget は "setGrid" オプションをオンにし
てありますので、ウィンドウをリサイズする時には高さと幅は常に文字高と文
字幅の整数倍になります。また、ウィンドウを狭くした場合には長い行が自動
的に折り返され、常に全ての内容が見えるようになっています。
EOT

  set_insert('0.0')
}

