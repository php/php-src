
Bench = %w(
  loop
  ack
  fib
  tak
  fact
)

Lang = <<EOP.map{|l| l.strip}
  ruby-cyg
  ../../../test6/miniruby
  perl
  python
  gosh
EOP

Bench.replace ['loop2']
Lang.replace ['ruby-cyg']

Ext = %w(
  .rb
  .rb
  .pl
  .py
  .scm
)

p Bench
p Lang

require 'benchmark'

def bench cmd
  m = Benchmark.measure{
    #p cmd
    system(cmd)
  }
  [m.utime, m.real]
end

Result = []
Bench.each{|b|
  r = []
  Lang.each_with_index{|l, idx|
    cmd = "#{l} #{b}#{Ext[idx]}"
    r << bench(cmd)
  }
  Result << r
}

require 'pp'
# utime
puts Lang.join("\t")
Bench.each_with_index{|b, bi|
  print b, "\t"
  puts Result[bi].map{|e| e[0]}.join("\t")
}

# rtime
puts Lang.join("\t")
Bench.each_with_index{|b, bi|
  print b, "\t"
  puts Result[bi].map{|e| e[1]}.join("\t")
}

