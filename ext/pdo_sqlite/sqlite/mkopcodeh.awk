#!/usr/bin/awk -f
#
# Generate the file opcodes.h.
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
# We go to the trouble of making some OP_ value the same as TK_ values
# as an optimization.  During parsing, things like expression operators
# are coded with TK_ values such as TK_ADD, TK_DIVIDE, and so forth.  Later
# during code generation, we need to generate corresponding opcodes like
# OP_Add and OP_Divide.  By making TK_ADD==OP_Add and TK_DIVIDE==OP_Divide,
# code to translation from one to the other is avoided.  This makes the
# code generator run (infinitesimally) faster and more importantly it makes
# the total library smaller.
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
      sameas[op[name]] = $(i+2)
    }
  }
}

# Assign numbers to all opcodes and output the result.
END {
  cnt = 0
  max = 0
  print "/* Automatically generated.  Do not edit */"
  print "/* See the mkopcodeh.awk script for details */"
  for(name in op){
    if( op[name]<0 ){
      cnt++
      while( used[cnt] ) cnt++
      op[name] = cnt
    }
    used[op[name]] = 1;
    if( op[name]>max ) max = op[name]
    printf "#define %-25s %15d", name, op[name]
    if( sameas[op[name]] ) {
      printf "   /* same as %-12s*/", sameas[op[name]]
    } 
    printf "\n"

  }
  seenUnused = 0;
  for(i=1; i<max; i++){
    if( !used[i] ){
      if( !seenUnused ){
        printf "\n/* The following opcode values are never used */\n"
        seenUnused = 1
      }
      printf "#define %-25s %15d\n", sprintf( "OP_NotUsed_%-3d", i ), i
    }
  }
}
