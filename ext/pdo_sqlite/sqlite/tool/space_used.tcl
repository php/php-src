# Run this TCL script using "testfixture" in order get a report that shows
# how much disk space is used by a particular data to actually store data
# versus how much space is unused.
#

# Get the name of the database to analyze
#
if {[llength $argv]!=1} {
  puts stderr "Usage: $argv0 database-name"
  exit 1
}
set file_to_analyze [lindex $argv 0]

# Open the database
#
sqlite db [lindex $argv 0]
set DB [btree_open [lindex $argv 0]]

# Output the schema for the generated report
#
puts \
{BEGIN;
CREATE TABLE space_used(
   name clob,        -- Name of a table or index in the database file
   is_index boolean, -- TRUE if it is an index, false for a table
   payload int,      -- Total amount of data stored in this table or index
   pri_pages int,    -- Number of primary pages used
   ovfl_pages int,   -- Number of overflow pages used
   pri_unused int,   -- Number of unused bytes on primary pages
   ovfl_unused int   -- Number of unused bytes on overflow pages
);}

# This query will be used to find the root page number for every index and
# table in the database.
#
set sql {
  SELECT name, type, rootpage FROM sqlite_master
  UNION ALL
  SELECT 'sqlite_master', 'table', 2
  ORDER BY 1
}

# Initialize variables used for summary statistics.
#
set total_size 0
set total_primary 0
set total_overflow 0
set total_unused_primary 0
set total_unused_ovfl 0

# Analyze every table in the database, one at a time.
#
foreach {name type rootpage} [db eval $sql] {
  set cursor [btree_cursor $DB $rootpage 0]
  set go [btree_first $cursor]
  set size 0
  catch {unset pg_used}
  set unused_ovfl 0
  set n_overflow 0
  while {$go==0} {
    set payload [btree_payload_size $cursor]
    incr size $payload
    set stat [btree_cursor_dump $cursor]
    set pgno [lindex $stat 0]
    set freebytes [lindex $stat 4]
    set pg_used($pgno) $freebytes
    if {$payload>238} {
      set n [expr {($payload-238+1019)/1020}]
      incr n_overflow $n
      incr unused_ovfl [expr {$n*1020+238-$payload}]
    }
    set go [btree_next $cursor]
  }
  btree_close_cursor $cursor
  set n_primary [llength [array names pg_used]]
  set unused_primary 0
  foreach x [array names pg_used] {incr unused_primary $pg_used($x)}
  regsub -all ' $name '' name
  puts -nonewline "INSERT INTO space_used VALUES('$name'"
  puts -nonewline ",[expr {$type=="index"}]"
  puts ",$size,$n_primary,$n_overflow,$unused_primary,$unused_ovfl);"
  incr total_size $size
  incr total_primary $n_primary
  incr total_overflow $n_overflow
  incr total_unused_primary $unused_primary
  incr total_unused_ovfl $unused_ovfl
}

# Output summary statistics:
#
puts "-- Total payload size: $total_size"
puts "-- Total pages used: $total_primary primary and $total_overflow overflow"
set file_pgcnt [expr {[file size [lindex $argv 0]]/1024}]
puts -nonewline "-- Total unused bytes on primary pages: $total_unused_primary"
if {$total_primary>0} {
  set upp [expr {$total_unused_primary/$total_primary}]
  puts " (avg $upp bytes/page)"
} else {
  puts ""
}
puts -nonewline "-- Total unused bytes on overflow pages: $total_unused_ovfl"
if {$total_overflow>0} {
  set upp [expr {$total_unused_ovfl/$total_overflow}]
  puts " (avg $upp bytes/page)"
} else {
  puts ""
}
set n_free [expr {$file_pgcnt-$total_primary-$total_overflow}]
if {$n_free>0} {incr n_free -1}
puts "-- Total pages on freelist: $n_free"
puts "COMMIT;"
