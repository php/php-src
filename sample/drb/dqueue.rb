=begin
 distributed Ruby --- Queue
 	Copyright (c) 1999-2000 Masatoshi SEKI
=end

require 'thread'
require 'drb/drb'

DRb.start_service(nil, Queue.new)
puts DRb.uri
DRb.thread.join

