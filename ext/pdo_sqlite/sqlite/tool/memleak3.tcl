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
SQLITE_DEBUG set to at least 2. It must be set to 3 to enable stack traces.

To use, run the leaky application and save the standard error output.
Then, execute this program with the first argument the name of the
application binary (or interpreter) and the second argument the name of the
text file that contains the collected stderr output.

If all goes well a summary of unfreed allocations is printed out. If the
GNU C library is in use and SQLITE_DEBUG is 3 or greater a stack trace is
printed out for each unmatched allocation.

Example:

$ ./testfixture ../sqlite/test/select1.test 2> memtrace.out
$ tclsh $argv0 ./testfixture memtrace.out
"

# If stack traces are enabled, the 'addr2line' program is called to
# translate a binary stack address into a human-readable form.
set addr2line addr2line

if { [llength $argv]!=2 } {
  puts "Usage: $argv0 <binary file> <mem trace file>"
  puts ""
  puts [string trim $doco]
  exit -1
}


proc process_input {input_file array_name} {
  upvar $array_name mem 
  set input [open $input_file]

  set MALLOC {([[:digit:]]+) malloc ([[:digit:]]+) bytes at 0x([[:xdigit:]]+)}
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

    } elseif { [regexp $FREE $line dummy bytes addr] } {
      # If this is a 'free' line, remove the entry from the mem array. If the 
      # entry does not exist, or is the wrong number of bytes, announce a
      # problem. This is more likely a bug in the regular expressions for
      # this script than an SQLite defect.
      if { [lindex $mem($addr) 0] != $bytes } {
        error "byte count mismatch"
      }
      unset mem($addr) 

    } elseif { [regexp $REALLOC $line dummy mallocid ob b oa a] } {
      # If it is a realloc line, remove the old mem entry and add a new one.
      unset mem($oa);
      set mem($a) [list $b "realloc $mallocid" $stack]
      set stack ""
    } else {
      # puts "REJECT: $line"
    }
  }

  close $input
}

process_input [lindex $argv 1] mem
set exe [lindex $argv 0]

foreach key [array names mem] {
  set bytes [lindex $mem($key) 0]
  set mallocid [lindex $mem($key) 1]
  set stack [lindex $mem($key) 2]
  puts "Leaked $bytes bytes at 0x$key: $mallocid"
  foreach frame [lrange $stack 1 10] {
    foreach {f l} [split [exec $addr2line -f --exe=$exe $frame] \n] {}
    puts [format "%-30s %s" $f $l]
  }
  if {[llength $stack]>0 } {puts ""}
}

