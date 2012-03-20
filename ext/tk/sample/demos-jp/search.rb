# -*- coding: utf-8 -*-
#
# Text Search widget demo (called by 'widget')
#

# textLoadFile --
# This method below loads a file into a text widget, discarding
# the previous contents of the widget. Tags for the old widget are
# not affected, however.
#
# Arguments:
# w -           The window into which to load the file.  Must be a
#               text widget.
# file -        The name of the file to load.  Must be readable.

def textLoadFile(w,file)
  w.delete('1.0', 'end')
  f = open(file, 'r')
  while(!f.eof?)
    w.insert('end', f.read(1000))
  end
  f.close
end

# textSearch --
# Search for all instances of a given string in a text widget and
# apply a given tag to each instance found.
#
# Arguments:
# w -           The window in which to search.  Must be a text widget.
# string -      The string to search for.  The search is done using
#               exact matching only;  no special characters.
# tag -         Tag to apply to each instance of a matching string.

def textSearch(w, string, tag)
  tag.remove('0.0', 'end')
  return if string == ""
  cur = '1.0'
  loop {
    cur, len = w.search_with_length(string, cur, 'end')
    break if cur == ""
    tag.add(cur, "#{cur} + #{len} char")
    cur = w.index("#{cur} + #{len} char")
  }
end

# textToggle --
# This method is invoked repeatedly to invoke two commands at
# periodic intervals.  It normally reschedules itself after each
# execution but if an error occurs (e.g. because the window was
# deleted) then it doesn't reschedule itself.
#
# Arguments:
# cmd1 -        Command to execute when method is called.
# sleep1 -      Ms to sleep after executing cmd1 before executing cmd2.
# cmd2 -        Command to execute in the *next* invocation of this method.
# sleep2 -      Ms to sleep after executing cmd2 before executing cmd1 again.

def textToggle(cmd1,sleep1,cmd2,sleep2)
  sleep_list = [sleep2, sleep1]
  TkAfter.new(proc{sleep = sleep_list.shift; sleep_list.push(sleep); sleep},
              -1, cmd1, cmd2).start(sleep1)
end

# toplevel widget が存在すれば削除する
if defined?($search_demo) && $search_demo
  $search_demo.destroy
  $search_demo = nil
end

# demo 用の toplevel widget を生成
$search_demo = TkToplevel.new {|w|
  title("Text Demonstration - Search and Highlight")
  iconname("search")
  positionWindow(w)
}

base_frame = TkFrame.new($search_demo).pack(:fill=>:both, :expand=>true)

# frame 生成
$search_buttons = TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    #text '了解'
    text '閉じる'
    command proc{
      tmppath = $search_demo
      $search_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'コード参照'
    command proc{showCode 'search'}
  }.pack('side'=>'left', 'expand'=>'yes')
}
$search_buttons.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# frame 生成
TkFrame.new(base_frame) {|f|
  TkLabel.new(f, 'text'=>'ファイル名:',
              'width'=>13, 'anchor'=>'w').pack('side'=>'left')
  $search_fileName = TkVariable.new
  TkEntry.new(f, 'width'=>40,
              'textvariable'=>$search_fileName) {
    pack('side'=>'left')
    bind('Return', proc{textLoadFile($search_text, $search_fileName.value)
                        $search_string_entry.focus})
    focus
  }
  TkButton.new(f, 'text'=>'読み込み',
               'command'=>proc{textLoadFile($search_text,
                                            $search_fileName.value)})\
  .pack('side'=>'left', 'pady'=>5, 'padx'=>10)
}.pack('side'=>'top', 'fill'=>'x')

TkFrame.new(base_frame) {|f|
  TkLabel.new(f, 'text'=>'検索文字列:',
              'width'=>13, 'anchor'=>'w').pack('side'=>'left')
  $search_searchString = TkVariable.new
  $search_string_entry = TkEntry.new(f, 'width'=>40,
                                     'textvariable'=>$search_searchString) {
    pack('side'=>'left')
    bind('Return', proc{textSearch($search_text, $search_searchString.value,
                                   $search_Tag)})
  }
  TkButton.new(f, 'text'=>'反転',
               'command'=>proc{textSearch($search_text,
                                          $search_searchString.value,
                                          $search_Tag)}) {
    pack('side'=>'left', 'pady'=>5, 'padx'=>10)
  }
}.pack('side'=>'top', 'fill'=>'x')

$search_text = TkText.new(base_frame, 'setgrid'=>true) {|t|
  $search_Tag = TkTextTag.new(t)
  TkScrollbar.new(base_frame, 'command'=>proc{|*args| t.yview(*args)}) {|sc|
    t.yscrollcommand(proc{|first,last| sc.set first,last})
    pack('side'=>'right', 'fill'=>'y')
  }
  pack('expand'=>'yes', 'fill'=>'both')
}

# Set up display styles for text highlighting.

if TkWinfo.depth($search_demo) > 1
  textToggle(proc{
               $search_Tag.configure('background'=>'#ce5555',
                                     'foreground'=>'white')
             },
             800,
             proc{
               $search_Tag.configure('background'=>'', 'foreground'=>'')
             },
             200 )
else
  textToggle(proc{
               $search_Tag.configure('background'=>'black',
                                     'foreground'=>'white')
             },
             800,
             proc{
               $search_Tag.configure('background'=>'', 'foreground'=>'')
             },
             200 )
end
$search_text.insert('1.0', "\
このウィンドウは検索機構を実現するのにテキスト widget のタグ機能がどの \
ように使われるのかをデモするものです。まず上のエントリにファイル名を入 \
れ、<リターン> を押すか「ロード」ボタンを押してください。次にその下の \
エントリに文字列を入力し、<リターン> を押すか「反転」ボタンを押してく \
ださい。するとファイル中の、検索文字列と一致する部分に全て \"search_Tag\" \
というタグがつけられ、タグの表示属性としてその文字列が点滅するように \
設定されます。\n")
$search_text.insert('end', "\
ファイル読み込みのカレントディレクトリは \"#{Dir.pwd}\" です。\
")
$search_text.set_insert '0.0'

$search_fileName.value = ''
$search_searchString.value = ''

$search_text.width = 60
$search_text.height = 20
