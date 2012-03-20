#! /usr/local/bin/ruby -vd

# tcltklib ライブラリのテスト

require "tcltklib"

def test
  # インタプリタを生成する
  ip1 = TclTkIp.new()

  # 評価してみる
  print ip1._return_value().inspect, "\n"
  print ip1._eval("puts {abc}").inspect, "\n"

  # ボタンを作ってみる
  print ip1._return_value().inspect, "\n"
  print ip1._eval("button .lab -text exit -command \"destroy .\"").inspect,
    "\n"
  print ip1._return_value().inspect, "\n"
  print ip1._eval("pack .lab").inspect, "\n"
  print ip1._return_value().inspect, "\n"

  # インタプリタから ruby コマンドを評価してみる
#  print ip1._eval(%q/ruby {print "print by ruby\n"}/).inspect, "\n"
  print ip1._eval(%q+puts [ruby {print "print by ruby\n"; "puts by tcl/tk"}]+).inspect, "\n"
  print ip1._return_value().inspect, "\n"

  # もう一つインタプリタを生成してみる
  ip2 = TclTkIp.new()
  ip2._eval("button .lab -text test -command \"puts test ; destroy .\"")
  ip2._eval("pack .lab")

  TclTkLib.mainloop
end

test
GC.start

print "exit\n"
