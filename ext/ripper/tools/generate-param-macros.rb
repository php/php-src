off = true
ARGF.each do |line|
  case line
  when /RIPPER_PARAMS_DECL_BEGIN/
    off = false
  when /RIPPER_PARAMS_DECL_END/
    exit
  when /ripper/
    next if off
    var = line.scan(/\w+/).last or next
    base = var.sub(/ripper_/, '')
    puts %"\#define #{base}\t\t(parser->ripper_#{base})"
  end
end
