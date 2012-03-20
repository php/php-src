#!/usr/bin/env ruby

require 'tk'
require 'tkextlib/vu/charts'

#######################################

Tk.root.geometry('+30+30')

delay = 2000

c = TkCanvas.new.pack

begin
  st = Tk::Vu::TkcSticker.new(c, 0, 0, 10, 10)
rescue
  Tk.messageBox(:type=>'ok', :title=>"No sticker Item",
                :message=>"This build of vu does not include the sticker item")
  exit
end
#st.delete

steps = []

steps << proc{
  # I used a 75dpi screen for testing, but others should make no difference!
  puts 'You\'ll see a small upright rectangle with "He" inside.'
  st = Tk::Vu::TkcSticker.new(c, '6m', '10m', '13m', '27m', :text=>'Hello')
}

steps << proc{
  puts 'You\'ll see the whole "Hello" drawn rotated 90 degrees.'
  st[:orient] = :vertical
}

steps << proc{
  puts 'The rectangle shrinks and the text is clipped to "Hell"'
  #st.coords('6m', '10m', '13m', '20m')
  st.coords('6m', '10m', '13m', '17m')
}

steps << proc{
  puts 'Now you\'ll read "ello"'
  st[:lefttrunc] = true
}

steps << proc{
  puts 'Enlarging the rectangle shows the complete "Hello" again'
  st.scale(0, 0, 3, 3)
}

steps << proc{
  puts 'This time the text is repeated: "Hello", approx. 5mm space, "Hello"'
  st[:space] = '5m'
}

steps << proc{
  puts 'A vertical bar appears in the lower right region and text jumps to the left.'
  st.configure(:anchor=>:n, :relw=>0.3, :relh=>0.7,
               :relx=>0.6, :rely=>0.3, :bar=>'red')
}

steps << proc{
  puts 'Paint the backgound.'
  st[:fill] = 'yellow'
}

steps << proc{
  puts "Let's test stippling."
  st[:stipple] = 'gray25'
}

steps << proc{
  puts 'Finally a large outline forces a single "Hello" and shrinks the bar.'
  st[:width] = '6m'
}

Tk.root.bind('q', proc{exit})

TkTimer.new(delay, 1, *steps).start

Tk.mainloop
