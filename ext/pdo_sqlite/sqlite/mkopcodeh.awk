#!/usr/bin/awk -f
#
# This AWK script scans a concatenation of the parse.h output file from the
# parser and the vdbe.c source file in order to generate the opcodes numbers
# for all opcodes.  
#
# The lines of the vdbe.c that we are interested in are of the form:
#
#       case OP_aaaa:      /* same as TK_bbbbb */
#
# The TK_ comment is optional.  If it is present, then the value assigned to
# the OP_ is the same as the TK_ value.  If missing, the OP_ value is assigned
# a small integer that is different from every other OP_ value.
#

# Remember the TK_ values from the parse.h file
/^#define TK_/ {
  tk[$2] = $3
}

# Scan for "case OP_aaaa:" lines in the vdbe.c file
/^case OP_/ {
  name = $2
  gsub(/:/,"",name)
  gsub("\r","",name)
  op[name] = -1
  for(i=3; i<NF-2; i++){
    if($i=="same" && $(i+1)=="as"){
      op[name] = tk[$(i+2)]
      used[op[name]] = 1
    }
  }
}

# Assign numbers to all opcodes and output the result.
END {
  cnt = 0
  print "/* Automatically generated.  Do not edit */"
  print "/* See the mkopcodeh.awk script for details */"
  for(name in op){
    if( op[name]<0 ){
      cnt++
      while( used[cnt] ) cnt++
      op[name] = cnt
    }
    printf "#define %-30s %d\n", name, op[name]
  }
}
