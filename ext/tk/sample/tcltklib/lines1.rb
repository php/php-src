#! /usr/local/bin/ruby

require "tcltk"

def drawlines()
  print Time.now, "\n"

  for j in 0 .. 99
    print "*"
    $stdout.flush
    if (j & 1) != 0
      col = "blue"
    else
      col = "red"
    end
    for i in 0 .. 99
#      $a.e("create line", i, 0, 0, 500 - i, "-fill", col)
    end
  end

  print Time.now, "\n"

  for j in 0 .. 99
    print "*"
    $stdout.flush
    if (j & 1) != 0
      col = "blue"
    else
      col = "red"
    end
    for i in 0 .. 99
      $a.e("create line", i, 0, 0, 500 - i, "-fill", col)
    end
  end

  print Time.now, "\n"
#  $ip.commands()["destroy"].e($root)
end

$ip = TclTkInterpreter.new()
$root = $ip.rootwidget()
$a = TclTkWidget.new($ip, $root, "canvas", "-height 500 -width 500")
$c = TclTkCallback.new($ip, proc{drawlines()})
$b = TclTkWidget.new($ip, $root, "button", "-text draw -command", $c)

$ip.commands()["pack"].e($a, $b, "-side left")

TclTk.mainloop

# eof
