# ARGF = open('ha')
cd = `pwd`.chomp + '/'
ARGF.each{|line|
  if /^0x([a-z0-9]+),/ =~ line
    stat = line.split(',')
    addr = stat[0].hex + 0x00400000
    retired = stat[2].to_i
    ticks   = stat[3].to_i

    src = `addr2line -e miniruby.exe #{addr.to_s(16)}`.chomp
    src.sub!(cd, '')
    puts '%-40s 0x%08x %8d %8d' % [src, addr, retired, ticks]
  end
}

