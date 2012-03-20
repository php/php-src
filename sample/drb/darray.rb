=begin
 distributed Ruby --- Array
 	Copyright (c) 1999-2001 Masatoshi SEKI
=end

require 'drb/drb'

here = ARGV.shift
DRb.start_service(here, [1, 2, "III", 4, "five", 6])
puts DRb.uri
DRb.thread.join

