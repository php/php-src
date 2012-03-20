# IO.select performance. worst case of single fd.

ios = []
nr = 1000000
if defined?(Process::RLIMIT_NOFILE)
  max = Process.getrlimit(Process::RLIMIT_NOFILE)[0]
else
  max = 64
end
puts "max fd: #{max} (results not apparent with <= 1024 max fd)"

((max / 2) - 10).times do
  ios.concat IO.pipe
end

last = [ ios[-1] ]
puts "last IO: #{last[0].inspect}"

nr.times do
  IO.select nil, last
end

