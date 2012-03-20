#! /usr/bin/ruby
5000.times do
  100.times do
    {"xxxx"=>"yyyy"}
  end
  GC.start
end
