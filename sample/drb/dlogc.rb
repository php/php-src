=begin
 distributed Ruby --- Log test
 	Copyright (c) 1999-2001 Masatoshi SEKI
=end

require 'drb/drb'

there = ARGV.shift || raise("usage: #{$0} <server_uri>")

DRb.start_service
ro = DRbObject.new(nil, there)
ro.log(123)
ro.log("hello")
sleep 2
ro.log("wakeup")

