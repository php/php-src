#
# This script looks for memory leaks by analyzing the output of "sqlite" 
# when compiled with the MEMORY_DEBUG=2 option.
#
/[0-9]+ malloc / {
  mem[$6] = $0
}
/[0-9]+ realloc / {
  mem[$8] = "";
  mem[$10] = $0
}
/[0-9]+ free / {
  if (mem[$6]=="") {
    print "*** free without a malloc at",$6
  }
  mem[$6] = "";
  str[$6] = ""
}
/^string at / {
  addr = $4
  sub("string at " addr " is ","")
  str[addr] = $0
}
END {
  for(addr in mem){
    if( mem[addr]=="" ) continue
    print mem[addr], str[addr]
  }
}
