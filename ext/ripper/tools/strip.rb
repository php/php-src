last_is_void = false
ARGF.each do |line|
  if line.strip.empty?
    #puts() unless last_is_void
    last_is_void = true
  elsif /\A\#/ === line
    ;
  else
    print line
    last_is_void = false
  end
end
