#
#
#

require 'benchmark'
require 'rbconfig'

$rubybin = ENV['RUBY'] || RbConfig.ruby

def runfile file
  puts file
  file = File.join(File.dirname($0), 'contrib', file)
  Benchmark.bm{|x|
    x.report('ruby'){
      system("#{$rubybin} #{file}")
    }
    x.report('yarv'){
      system("#{$rubybin} -rite -I.. #{file}")
    }
  }
end

ARGV.each{|file|
  runfile file
}


