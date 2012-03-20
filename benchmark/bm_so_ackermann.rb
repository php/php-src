#!/usr/bin/ruby
# -*- mode: ruby -*-
# $Id: ackermann-ruby.code,v 1.4 2004/11/13 07:40:41 bfulgham Exp $
# http://www.bagley.org/~doug/shootout/

def ack(m, n)
    if m == 0 then
        n + 1
    elsif n == 0 then
        ack(m - 1, 1)
    else
        ack(m - 1, ack(m, n - 1))
    end
end

NUM = 9
ack(3, NUM)


