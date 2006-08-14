#/bin/sh
# \
exec `which tclsh` $0 "$@"
#
# The author disclaims copyright to this source code.  In place of
# a legal notice, here is a blessing:
#
#    May you do good and not evil.
#    May you find forgiveness for yourself and forgive others.
#    May you share freely, never taking more than you give.
######################################################################

set doco "
This script is a tool to help track down memory leaks in the sqlite
library. The library must be compiled with the preprocessor symbol
SQLITE_MEMDEBUG set to at least 2. It must be set to 3 to enable stack 
traces.

To use, run the leaky application and save the standard error output.
Then, execute this program with the first argument the name of the
application binary (or interpreter) and the second argument the name of the
text file that contains the collected stderr output.

If all goes well a summary of unfreed allocations is printed out. If the
GNU C library is in use and SQLITE_DEBUG is 3 or greater a stack trace is
printed out for each unmatched allocation.

If the \"-r <n>\" option is passed, then the program stops and prints out
the state of the heap immediately after the <n>th call to malloc() or
realloc().

Example:

$ ./testfixture ../sqlite/test/select1.test 2> memtrace.out
$ tclsh $argv0 ?-r <malloc-number>? ./testfixture memtrace.out
"


proc usage {} {
  set prg [file tail $::argv0]
  puts "Usage: $prg ?-r <malloc-number>? <binary file> <mem trace file>"
  puts ""
  puts [string trim $::doco]
  exit -1
}

proc shift {listvar} {
  upvar $listvar l
  set ret [lindex $l 0]
  set l [lrange $l 1 end]
  return $ret
}

# Argument handling. The following vars are set:
#
# $exe       - the name of the executable (i.e. "testfixture" or "./sqlite3")
# $memfile   - the name of the file containing the trace output.
# $report_at - The malloc number to stop and report at. Or -1 to read 
#              all of $memfile.
#
set report_at -1
while {[llength $argv]>2} {
  set arg [shift argv]
  switch -- $arg {
    "-r" {
      set report_at [shift argv]
    }
    default {
      usage
    }
  }
}
if {[llength $argv]!=2} usage
set exe [lindex $argv 0]
set memfile [lindex $argv 1]

# If stack traces are enabled, the 'addr2line' program is called to
# translate a binary stack address into a human-readable form.
set addr2line addr2line

# When the SQLITE_MEMDEBUG is set as described above, SQLite prints
# out a line for each malloc(), realloc() or free() call that the
# library makes. If SQLITE_MEMDEBUG is 3, then a stack trace is printed
# out before each malloc() and realloc() line.
#
# This program parses each line the SQLite library outputs and updates
# the following global Tcl variables to reflect the "current" state of
# the heap used by SQLite.
#
set nBytes 0               ;# Total number of bytes currently allocated.
set nMalloc 0              ;# Total number of malloc()/realloc() calls.
set nPeak 0                ;# Peak of nBytes.
set iPeak 0                ;# nMalloc when nPeak was set.
#
# More detailed state information is stored in the $memmap array. 
# Each key in the memmap array is the address of a chunk of memory
# currently allocated from the heap. The value is a list of the 
# following form
# 
#     {<number-of-bytes> <malloc id> <stack trace>}
#
array unset memmap

proc process_input {input_file array_name} {
  upvar $array_name mem 
  set input [open $input_file]

  set MALLOC {([[:digit:]]+) malloc ([[:digit:]]+) bytes at 0x([[:xdigit:]]+)}
  # set STACK {^[[:digit:]]+: STACK: (.*)$}
  set STACK {^STACK: (.*)$}
  set FREE {[[:digit:]]+ free ([[:digit:]]+) bytes at 0x([[:xdigit:]]+)}
  set REALLOC {([[:digit:]]+) realloc ([[:digit:]]+) to ([[:digit:]]+)}
  append REALLOC { bytes at 0x([[:xdigit:]]+) to 0x([[:xdigit:]]+)}

  set stack ""
  while { ![eof $input] } {
    set line [gets $input]
    if {[regexp $STACK $line dummy stack]} {
      # Do nothing. The variable $stack now stores the hexadecimal stack dump
      # for the next malloc() or realloc().

    } elseif { [regexp $MALLOC $line dummy mallocid bytes addr]  } {
      # If this is a 'malloc' line, set an entry in the mem array. Each entry
      # is a list of length three, the number of bytes allocated , the malloc
      # number and the stack dump when it was allocated.
      set mem($addr) [list $bytes "malloc $mallocid" $stack]
      set stack ""

      # Increase the current heap usage
      incr ::nBytes $bytes

      # Increase the number of malloc() calls
      incr ::nMalloc

      if {$::nBytes > $::nPeak} {
        set ::nPeak $::nBytes
        set ::iPeak $::nMalloc
      }

    } elseif { [regexp $FREE $line dummy bytes addr] } {
      # If this is a 'free' line, remove the entry from the mem array. If the 
      # entry does not exist, or is the wrong number of bytes, announce a
      # problem. This is more likely a bug in the regular expressions for
      # this script than an SQLite defect.
      if { [lindex $mem($addr) 0] != $bytes } {
        error "byte count mismatch"
      }
      unset mem($addr) 

      # Decrease the current heap usage
      incr ::nBytes [expr -1 * $bytes]

    } elseif { [regexp $REALLOC $line dummy mallocid ob b oa a] } {
      # "free" the old allocation in the internal model:
      incr ::nBytes [expr -1 * $ob]
      unset mem($oa);

      # "malloc" the new allocation
      set mem($a) [list $b "realloc $mallocid" $stack]
      incr ::nBytes $b
      set stack ""

      # Increase the number of malloc() calls
      incr ::nMalloc

      if {$::nBytes > $::nPeak} {
        set ::nPeak $::nBytes
        set ::iPeak $::nMalloc
      }

    } else {
      # puts "REJECT: $line"
    }

    if {$::nMalloc==$::report_at} report
  }

  close $input
}

proc printstack {stack} {
  set fcount 10
  if {[llength $stack]<10} {
    set fcount [llength $stack]
  }
  foreach frame [lrange $stack 1 $fcount] {
    foreach {f l} [split [exec $::addr2line -f --exe=$::exe $frame] \n] {}
    puts [format "%-30s %s" $f $l]
  }
  if {[llength $stack]>0 } {puts ""}
}

proc report {} {

  foreach key [array names ::memmap] {
    set stack [lindex $::memmap($key) 2]
    set bytes [lindex $::memmap($key) 0]
    lappend summarymap($stack) $bytes
  }

  set sorted [list]
  foreach stack [array names summarymap] {
    set allocs $summarymap($stack)
    set sum 0
    foreach a $allocs {
      incr sum $a
    }
    lappend sorted [list $sum $stack]
  }

  set sorted [lsort -integer -index 0 $sorted]
  foreach s $sorted {
    set sum [lindex $s 0]
    set stack [lindex $s 1]
    set allocs $summarymap($stack)
    puts "$sum bytes in [llength $allocs] chunks ($allocs)"
    printstack $stack
  }

  # Print out summary statistics
  puts "Total allocations            : $::nMalloc"
  puts "Total outstanding allocations: [array size ::memmap]" 
  puts "Current heap usage           : $::nBytes bytes"
  puts "Peak heap usage              : $::nPeak bytes (malloc #$::iPeak)"

  exit
}

process_input $memfile memmap
report



