#
# YARV benchmark driver
#

require 'yarvutil'
require 'benchmark'
require 'rbconfig'

def exec_command type, file, w
  <<-EOP
  $DRIVER_PATH = '#{File.dirname($0)}'
  $LOAD_PATH.replace $LOAD_PATH | #{$LOAD_PATH.inspect}
  require 'benchmark'
  require 'yarvutil'
#  print '#{type}'
  begin
    puts Benchmark.measure{
      #{w}('#{file}')
    }.utime
  rescue Exception => exec_command_error_variable
    puts "\t" + exec_command_error_variable.message
  end
  EOP
end

def benchmark cmd
  rubybin = ENV['RUBY'] || RbConfig.ruby

  IO.popen(rubybin, 'r+'){|io|
    io.write cmd
    io.close_write
    return io.gets
  }
end

def ruby_exec file
  prog = exec_command 'ruby', file, 'load'
  benchmark prog
end

def yarv_exec file
  prog = exec_command 'yarv', file, 'YARVUtil.load_bm'
  benchmark prog
end

$wr = $wy = nil

def measure bench
  file = File.dirname($0) + "/bm_#{bench}.rb"
  r = ruby_exec(file).to_f
  y = yarv_exec(file).to_f
  puts "#{bench}\t#{r}\t#{y}"
end

def measure2
  r = ruby_exec.to_f
  y = yarv_exec.to_f
  puts r/y
end

if $0 == __FILE__
  %w{
    whileloop
    whileloop2
    times
    const
    method
    poly_method
    block
    rescue
    rescue2
  }.each{|bench|
    measure bench
  }
end




