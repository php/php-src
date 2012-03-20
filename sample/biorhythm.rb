#!/usr/local/bin/ruby
#
#               biorhythm.rb -
#                       $Release Version: $
#                       $Revision$
#                       by Yasuo OHBA(STAFS Development Room)
#
# --
#
#
#

# probably based on:
#
# Newsgroups: comp.sources.misc,de.comp.sources.os9
# From: fkk@stasys.sta.sub.org (Frank Kaefer)
# Subject: v41i126:  br - Biorhythm v3.0, Part01/01
# Message-ID: <1994Feb1.070616.15982@sparky.sterling.com>
# Sender: kent@sparky.sterling.com (Kent Landfield)
# Organization: Sterling Software
# Date: Tue, 1 Feb 1994 07:06:16 GMT
#
# Posting-number: Volume 41, Issue 126
# Archive-name: br/part01
# Environment: basic, dos, os9

include Math
require "date.rb"
require "optparse"
require "optparse/date"

def print_header(y, m, d, p, w)
  print "\n>>> Biorhythm <<<\n"
  printf "The birthday %04d.%02d.%02d is a %s\n", y, m, d, w
  printf "Age in days: [%d]\n\n", p
end

def get_position(z)
  pi = Math::PI
  z = Integer(z)
  phys = (50.0 * (1.0 + sin((z / 23.0 - (z / 23)) * 360.0 * pi / 180.0))).to_i
  emot = (50.0 * (1.0 + sin((z / 28.0 - (z / 28)) * 360.0 * pi / 180.0))).to_i
  geist =(50.0 * (1.0 + sin((z / 33.0 - (z / 33)) * 360.0 * pi / 180.0))).to_i
  return phys, emot, geist
end

def prompt(msg)
  $stderr.print msg
  return gets.chomp
end

#
# main program
#
options = {
  :graph => true,
  :date  => Date.today,
  :days  => 9,
}
ARGV.options do |opts|
  opts.on("-b", "--birthday=DATE", Date, "specify your birthday"){|v|
    options[:birthday] = v
  }
  opts.on("--date=DATE", Date, "specify date to show"){|v|
    options[:date] = v
  }
  opts.on("-g", "--show-graph", TrueClass, "show graph (default)"){|v|
    options[:graph] = v
  }
  opts.on("-v", "--show-values", TrueClass, "show values"){|v|
    options[:graph] = !v
  }
  opts.on("--days=DAYS", Integer, "graph range (only in effect for graph)"){|v|
    options[:days] = v - 1
  }
  opts.on_tail("-h", "--help", "show this message"){puts opts; exit}
  begin
    opts.parse!
  rescue => ex
    puts "Error: #{ex.message}"
    puts opts
    exit
  end
end

bd = options[:birthday] || Date.parse(prompt("Your birthday (YYYYMMDD): "))
dd = options[:date] || Date.today
ausgabeart = options[:graph] ? "g" : "v"
display_period = options[:days]

if ausgabeart == "v"
  print_header(bd.year, bd.month, bd.day, dd - bd, bd.strftime("%a"))
  print "\n"

  phys, emot, geist = get_position(dd - bd)
  printf "Biorhythm:   %04d.%02d.%02d\n", dd.year, dd.month, dd.day
  printf "Physical:    %d%%\n", phys
  printf "Emotional:   %d%%\n", emot
  printf "Mental:      %d%%\n", geist
  print "\n"
else
  print_header(bd.year, bd.month, bd.day, dd - bd, bd.strftime("%a"))
  print "                     P=physical, E=emotional, M=mental\n"
  print "             -------------------------+-------------------------\n"
  print "                     Bad Condition    |    Good Condition\n"
  print "             -------------------------+-------------------------\n"

  (dd - bd).step(dd - bd + display_period) do |z|
    phys, emot, geist = get_position(z)

    printf "%04d.%02d.%02d : ", dd.year, dd.month, dd.day
    p = (phys / 2.0 + 0.5).to_i
    e = (emot / 2.0 + 0.5).to_i
    g = (geist / 2.0 + 0.5).to_i
    graph = "." * 51
    graph[25] = ?|
    graph[p] = ?P
    graph[e] = ?E
    graph[g] = ?M
    print graph, "\n"
    dd = dd + 1
  end
  print "             -------------------------+-------------------------\n\n"
end
