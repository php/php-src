#! ./ruby
# split into multi part
# usage: mpart.rb [-nnn] file..

lines = 1000

if (ARGV[0] =~ /^-(\d+)$/ )
  lines = $1.to_i;
  ARGV.shift;
end

basename = ARGV[0]
extname = "part"

part = 1
line = 0

fline = 0
for i in ifp = open(basename)
  fline = fline + 1
end
ifp.close

parts = fline / lines + 1

for i in ifp = open(basename)
  if line == 0
    ofp = open(sprintf("%s.%s%02d", basename, extname, part), "w")
    printf(ofp, "%s part%02d/%02d\n", basename, part, parts)
    ofp.write("BEGIN--cut here--cut here\n")
  end
  ofp.write(i)
  line = line + 1
  if line >= lines and !ifp.eof?
    ofp.write("END--cut here--cut here\n")
    ofp.close
    part = part + 1
    line = 0
  end
end
ofp.write("END--cut here--cut here\n")
ofp.close

ifp.close
