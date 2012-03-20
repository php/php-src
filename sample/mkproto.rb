$/ = nil
while line = gets()
  if /^((void|VALUE|int|char *\*|ID|struct \w+ *\*|st_table *\*) *)?\n(\w+)\(.*\)\n\s*((.+;\n)*)\{/ =~ line
    line = $'
    printf "%s %s(", $2, $3
    args = []
    for arg in $4.split(/;\n\s*/)
      arg.gsub!(/ +/, ' ')
      if arg =~ /,/
	if arg =~ /(([^*]+) *\** *\w+),/
	  type = $2.strip
	  args.push $1.strip
	  arg = $'
	else
	  type = ""
	end
	while arg.sub!(/(\** *\w+)(,|$)/, "") && $~
	  args.push type + " " + $1.strip
	end
      else
	args.push arg.strip
      end
    end
    printf "%s);\n", args.join(', ')
    redo
  end
end
