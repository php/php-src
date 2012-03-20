require 'drb/drb'
require 'drb/unix'
require 'drb/gw'

DRb.install_id_conv(DRb::GWIdConv.new)
gw = DRb::GW.new
s1 = DRb::DRbServer.new(ARGV.shift, gw)
s2 = DRb::DRbServer.new(ARGV.shift, gw)
s1.thread.join
s2.thread.join
