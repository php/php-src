line = ''
indent = 0
$stdout.sync = true
print "ruby> "
loop do
  l = gets
  if l.nil?
    break if line.empty?
  else
    line += l
    if l =~ /,\s*$/
      print "ruby| "
      next
    end
    if l =~ /^\s*(class|module|def|if|unless|case|while|until|for|begin)\b[^_]/
      indent += 1
    end
    if l =~ /^\s*end\b[^_]/
      indent -= 1
    end
    if l =~ /\{\s*(\|.*\|)?\s*$/
      indent += 1
    end
    if l =~ /^\s*\}/
      indent -= 1
    end
    if indent > 0
      print "ruby| "
      next
    end
  end
  begin
    print eval(line).inspect, "\n"
  rescue ScriptError, StandardError
    printf "ERR: %s\n", $! || 'exception raised'
  end
  break if l.nil?
  line = ''
  print "ruby> "
end
print "\n"
