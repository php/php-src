# -*- coding: utf-8 -*-
=begin
 distributed Ruby --- dRuby Sample Client -- chasen client
 	Copyright (c) 1999-2001 Masatoshi SEKI
=end

require 'drb/drb'

there = ARGV.shift || raise("usage: #{$0} <server_uri>")
DRb.start_service
dhasen = DRbObject.new(nil, there)

print dhasen.sparse("本日は、晴天なり。", "-F", '(%BB %m %M)\n', "-j")
print dhasen.sparse("本日は、晴天なり。", "-F", '(%m %M)\n')
