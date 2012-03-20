#!/usr/bin/env ruby
# This script generates a counter with start and stop buttons.

require "tk"
$label = TkLabel.new {
  text '0.00'
  relief 'raised'
  width 10
  pack('side'=>'bottom', 'fill'=>'both')
}

TkButton.new {
  text 'Start'
  command proc {
    if $stopped
      $stopped = FALSE
      tick
    end
  }
  pack('side'=>'left','fill'=>'both','expand'=>'yes')
}
TkButton.new {
  text 'Stop'
  command proc{
    exit if $stopped
    $stopped = TRUE
  }
  pack('side'=>'right','fill'=>'both','expand'=>'yes')
}

$seconds=0
$hundredths=0
$stopped=TRUE

def tick
  if $stopped then return end
  Tk.after 50, proc{tick}
  $hundredths+=5
  if $hundredths >= 100
    $hundredths=0
    $seconds+=1
  end
  $label.text format("%d.%02d", $seconds, $hundredths)
end

root = Tk.root
root.bind "Control-c", proc{root.destroy}
root.bind "Control-q", proc{root.destroy}
Tk.root.focus
Tk.mainloop
