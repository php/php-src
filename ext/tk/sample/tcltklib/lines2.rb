#! /usr/local/bin/ruby

require "tk"

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
#      TkcLine.new($a, i, 0, 0, 500 - i, "-fill", col)
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
      TkcLine.new($a, i, 0, 0, 500 - i, "-fill", col)
    end
  end

  print Time.now, "\n"
#  Tk.root.destroy
end

$a = TkCanvas.new{
  height(500)
  width(500)
}

$b = TkButton.new{
  text("draw")
  command(proc{drawlines()})
}

TkPack.configure($a, $b, {"side"=>"left"})

Tk.mainloop

# eof
