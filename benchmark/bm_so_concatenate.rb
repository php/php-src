#!/usr/bin/ruby
# -*- mode: ruby -*-
# $Id: strcat-ruby.code,v 1.4 2004/11/13 07:43:28 bfulgham Exp $
# http://www.bagley.org/~doug/shootout/
# based on code from Aristarkh A Zagorodnikov and Dat Nguyen

STUFF = "hello\n"
i=0
while i<10
  i+=1
  hello = ''
  4_000_000.times do |e|
    hello << STUFF
  end
end
# puts hello.length


