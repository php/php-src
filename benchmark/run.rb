#
# Ruby benchmark driver
#

require 'benchmark'
require 'rbconfig'

$matzrubyonly = false
$rubyonly = false

$results  = []

# prepare 'wc.input'
def prepare_wc_input
  wcinput = File.join(File.dirname($0), 'wc.input')
  wcbase  = File.join(File.dirname($0), 'wc.input.base')
  unless FileTest.exist?(wcinput)
    data = File.read(wcbase)
    13.times{
      data << data
    }
    open(wcinput, 'w'){|f| f.write data}
  end
end

prepare_wc_input

def bm file
  prog = File.readlines(file).map{|e| e.rstrip}.join("\n")
  return if prog.empty?

  /[a-z]+_(.+)\.rb/ =~ file
  bm_name = $1
  puts '-----------------------------------------------------------' unless $rubyonly || $matzrubyonly
  puts "#{bm_name}: "


puts <<EOS unless $matzrubyonly || $rubyonly
#{prog}
--
EOS
  begin
    result = [bm_name]
    result << matzruby_exec(file) unless $rubyonly
    result << ruby_exec(file) unless $matzrubyonly
    $results << result

  rescue Exception => e
    puts
    puts "** benchmark failure: #{e}"
    puts e.backtrace
  end
end

def benchmark file, bin
  m = Benchmark.measure{
    `#{bin} #{$opts} #{file}`
  }
  sec = '%.3f' % m.real
  puts " #{sec}"
  sec
end

def ruby_exec file
  print 'ruby'
  benchmark file, $ruby_program
end

def matzruby_exec file
  print 'matz'
  rubylib = ENV['RUBYLIB']
  ENV['RUBYLIB'] = ''
  r = benchmark file, $matzruby_program
  ENV['RUBYLIB'] = rubylib
  r
end

if $0 == __FILE__
  ARGV.each{|arg|
    case arg
    when /\A--ruby=(.+)/
      $ruby_program = $1
    when /\A--matzruby=(.+)/
      $matzruby_program = $1
    when /\A--opts=(.+)/
      $opts = $1
    when /\A(-r|--only-ruby)\z/
      $rubyonly = true
    when /\A(-m|--only-matzruby)\z/
      $matzrubyonly = true
    end
  }
  ARGV.delete_if{|arg|
    /\A-/ =~ arg
  }

  puts "MatzRuby:"
  system("#{$matzruby_program} -v")
  puts "Ruby:"
  system("#{$ruby_program} -v")
  puts

  if ARGV.empty?
    Dir.glob(File.dirname(__FILE__) + '/bm_*.rb').sort.each{|file|
      bm file
    }
  else
    ARGV.each{|file|
      Dir.glob(File.join(File.dirname(__FILE__), file + '*')){|ef|
        # file = "#{File.dirname(__FILE__)}/#{file}.rb"
        bm ef
      }
    }
  end

  puts
  puts "-- benchmark summary ---------------------------"
  $results.each{|res|
    print res.shift, "\t"
    (res||[]).each{|result|
      /([\d\.]+)/ =~ result
      print $1 + "\t" if $1
    }
    puts
  }
end

