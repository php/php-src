
require './rbconfig'
require 'fileutils'
require 'pp'

Ruby = ENV['RUBY'] || RbConfig.ruby
#

OPTIONS = %w{
 opt-direct-threaded-code
 opt-basic-operations
 opt-operands-unification
 opt-instructions-unification
 opt-inline-method-cache
 opt-stack-caching
}.map{|opt|
  '--disable-' + opt
}

opts = OPTIONS.dup
Configs = OPTIONS.map{|opt|
  o = opts.dup
  opts.delete(opt)
  o
} + [[]]

pp Configs if $DEBUG


def exec_cmd(cmd)
  puts cmd
  unless system(cmd)
    p cmd
    raise "error"
  end
end

def dirname idx
  "ev-#{idx}"
end

def build
  Configs.each_with_index{|config, idx|
    dir = dirname(idx)
    FileUtils.rm_rf(dir) if FileTest.exist?(dir)
    Dir.mkdir(dir)
    FileUtils.cd(dir){
      exec_cmd("#{Ruby} ../extconf.rb " + config.join(" "))
      exec_cmd("make clean test-all")
    }
  }
end

def check
  Configs.each_with_index{|c, idx|
    puts "= #{idx}"
    system("#{Ruby} -r ev-#{idx}/yarvcore -e 'puts YARVCore::OPTS'")
  }
end

def bench_each idx
  puts "= #{idx}"
  5.times{|count|
    print count
    FileUtils.cd(dirname(idx)){
      exec_cmd("make benchmark OPT=-y ITEMS=#{ENV['ITEMS']} > ../b#{idx}-#{count}")
    }
  }
  puts
end

def bench
  # return bench_each(6)
  Configs.each_with_index{|c, idx|
    bench_each idx
  }
end

def parse_result data
  flag = false
  stat = []
  data.each{|line|
    if flag
      if /(\w+)\t([\d\.]+)/ =~ line
        stat << [$1, $2.to_f]
      else
        raise "not a data"
      end

    end
    if /benchmark summary/ =~ line
      flag = true
    end
  }
  stat
end

def calc_each data
  data.sort!
  data.pop   # remove max
  data.shift # remove min

  data.inject(0.0){|res, e|
    res += e
  } / data.size
end

def calc_stat stats
  stat = []
  stats[0].each_with_index{|e, idx|
    bm = e[0]
    vals = stats.map{|st|
      st[idx][1]
    }
    [bm, calc_each(vals)]
  }
end

def stat
  total = []
  Configs.each_with_index{|c, idx|
    stats = []
    5.times{|count|
      file = "b#{idx}-#{count}"
      # p file
      open(file){|f|
        stats << parse_result(f.read)
      }
    }
    # merge stats
    total << calc_stat(stats)
    total
  }
  # pp total
  total[0].each_with_index{|e, idx|
    bm = e[0]
    # print "#{bm}\t"
    total.each{|st|
      print st[idx][1], "\t"
    }
    puts
  }
end

ARGV.each{|cmd|
  case cmd
  when 'build'
    build
  when 'check'
    check
  when 'bench'
    bench
  when 'stat'
    stat
  else
    raise
  end
}

