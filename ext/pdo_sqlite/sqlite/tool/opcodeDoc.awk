#
# Extract opcode documentation for sqliteVdbe.c and generate HTML
#
BEGIN {
  print "<html><body bgcolor=white>"
  print "<h1>SQLite Virtual Database Engine Opcodes</h1>"
  print "<table>"
}
/ Opcode: /,/\*\// {
  if( $2=="Opcode:" ){
    printf "<tr><td>%s&nbsp;%s&nbsp;%s&nbsp;%s</td>\n<td>\n", $3, $4, $5, $6
  }else if( $1=="*/" ){
    printf "</td></tr>\n"
  }else if( NF>1 ){
    sub(/^ *\*\* /,"")
    gsub(/</,"&lt;")
    gsub(/&/,"&amp;")
    print
  }
}
END {
  print "</table></body></html>"
}
