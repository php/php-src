# This AWK script reads the output of testfixture when compiled for memory
# debugging.  It generates SQL commands that can be fed into an sqlite 
# instance to determine what memory is never freed.  A typical usage would
# be as follows:
#
#     make -f memleak.mk fulltest 2>mem.out
#     awk -f ../sqlite/tool/memleak2.awk mem.out | ./sqlite :memory:
#
# The job performed by this script is the same as that done by memleak.awk.
# The difference is that this script uses much less memory when the size
# of the mem.out file is huge.
#
BEGIN {
  print "CREATE TABLE mem(loc INTEGER PRIMARY KEY, src);"
}
/[0-9]+ malloc / {
  print "INSERT INTO mem VALUES(" strtonum($6) ",'" $0 "');"
}
/[0-9]+ realloc / {
  print "INSERT INTO mem VALUES(" strtonum($10) \
           ",(SELECT src FROM mem WHERE loc=" strtonum($8) "));"
  print "DELETE FROM mem WHERE loc=" strtonum($8) ";"
}
/[0-9]+ free / {
  print "DELETE FROM mem WHERE loc=" strtonum($6) ";"
}
END {
  print "SELECT src FROM mem;"
}
