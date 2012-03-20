require 'erb'
require 'optparse'
require 'fileutils'

timestamp = nil
output = nil
ifchange = nil
opt = OptionParser.new do |o|
  o.on('-t', '--timestamp[=PATH]') {|v| timestamp = v || true}
  o.on('-o', '--output=PATH') {|v| output = v}
  o.on('-c', '--[no-]if-change') {|v| ifchange = v}
  o.order!(ARGV)
end
template = ARGV.shift or abort opt.to_s
erb = ERB.new(File.read(template), nil, '%')
erb.filename = template
result = erb.result
if output
  if ifchange and (IO.read(output) rescue nil) == result
    puts "#{output} unchanged"
  else
    open(output, "wb") {|f| f.print result}
    puts "#{output} updated"
  end
  if timestamp
    if timestamp == true
      dir, base = File.split(output)
      timestamp = File.join(dir, ".time." + base)
    end
    FileUtils.touch(timestamp)
  end
else
  print result
end
