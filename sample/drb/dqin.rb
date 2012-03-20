=begin
 distributed Ruby --- store
 	Copyright (c) 1999-2000 Masatoshi SEKI
=end

require 'drb/drb'
require 'dqlib'

there = ARGV.shift || raise("usage: #{$0} <server_uri>")

DRb.start_service
queue = DRbObject.new(nil, there)
queue.push(DQEntry.new(DRb.uri))
