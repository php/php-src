#!/usr/bin/env ruby

if ARGV[0] == "-c"
  out_stdout = 1
  ARGV.shift
end

$sawbegin = 0
$sawend = 0

while line = gets()
  if /^begin\s*(\d*)\s*(\S*)/ =~ line
    $mode, $file = $1, $2
    $sawbegin+=1
    if out_stdout
      out = STDOUT
    else
      out = open($file, "w") if $file != ""
    end
    out.binmode
    break
  end
end

raise "missing begin" unless $sawbegin

out.binmode
while line = gets()
  if /^end/ =~ line
    $sawend+=1
    out.close unless out_stdout
    File.chmod $mode.oct, $file unless out_stdout
    next
  end
  line.sub!(/[a-z]+$/, "")	# handle stupid trailing lowercase letters
  next if /[a-z]/ =~ line
  next if !(((($_[0] - 32) & 077) + 2) / 3 == $_.length / 4)
  out << $_.unpack("u") if $sawbegin > $sawend
end

raise "missing end" if $sawbegin > $sawend
raise "missing begin" if ! $sawbegin
exit 0
