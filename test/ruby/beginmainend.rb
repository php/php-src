errout = ARGV.shift

BEGIN {
  puts "b1"
  local_begin1 = "local_begin1"
  $global_begin1 = "global_begin1"
  ConstBegin1 = "ConstBegin1"
}

BEGIN {
  puts "b2"

  BEGIN {
    puts "b2-1"
  }
}

# for scope check
#raise if defined?(local_begin1)
raise unless defined?($global_begin1)
raise unless defined?(::ConstBegin1)
local_for_end2 = "e2"
$global_for_end1 = "e1"

puts "main"

END {
  puts local_for_end2	# e2
}

eval <<EOE
  BEGIN {
    puts "b3"

    BEGIN {
      puts "b3-1"
    }
  }

  BEGIN {
    puts "b4"
  }

  END {
    puts "e3"
  }

  END {
    puts "e4"

    END {
      puts "e4-1"

      END {
	puts "e4-1-1"
      }
    }

    END {
      puts "e4-2"
    }
  }
EOE

END {
  exit
  puts "should not be dumped"

  END {
    puts "not reached"
  }
}

END {
  puts $global_for_end1	# e1

  END {
    puts "e1-1"
  }
}
