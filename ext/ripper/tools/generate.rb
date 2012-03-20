# $Id$

require 'optparse'

def main
  mode = nil
  ids1src = nil
  ids2src = nil
  output = nil

  parser = @parser = OptionParser.new
  parser.banner = "Usage: #{File.basename($0)} --mode=MODE [--ids1src=PATH] [--ids2src=PATH] [--output=PATH]"
  parser.on('--mode=MODE', 'check, eventids1, or eventids2table.') {|m|
    mode = m
  }
  parser.on('--ids1src=PATH', 'A source file of event-IDs 1 (parse.y).') {|path|
    ids1src = path
  }
  parser.on('--ids2src=PATH', 'A source file of event-IDs 2 (eventids2.c).') {|path|
    ids2src = path
  }
  parser.on('--output=PATH', 'An output file.') {|path|
    output = path
  }
  parser.on('--help', 'Prints this message and quit.') {
    puts parser.help
    exit true
  }
  begin
    parser.parse!
  rescue OptionParser::ParseError => err
    usage err.message
  end
  usage 'no mode given' unless mode
  case mode
  when 'check'
    usage 'no --ids1src' unless ids1src
    usage 'no --ids2src' unless ids2src
    h = read_ids1_with_locations(ids1src)
    check_arity h
    ids2 = read_ids2(ids2src)
    common = h.keys & ids2
    unless common.empty?
      abort "event crash: #{common.join(' ')}"
    end
    exit 0
  when 'eventids1'
    usage 'no --ids1src' unless ids1src
    result = generate_eventids1(read_ids1(ids1src))
  when 'eventids2table'
    usage 'no --ids2src' unless ids2src
    result = generate_eventids2_table(read_ids2(ids2src))
  end
  if output
    File.open(output, 'w') {|f|
      f.write result
    }
  else
    puts result
  end
end

def usage(msg)
  $stderr.puts msg
  $stderr.puts @parser.help
  exit false
end

def generate_eventids1(ids)
  buf = ""
  ids.each do |id, arity|
    buf << %Q[static ID ripper_id_#{id};\n]
  end
  buf << %Q[\n]
  buf << %Q[static void\n]
  buf << %Q[ripper_init_eventids1(void)\n]
  buf << %Q[{\n]
  ids.each do |id, arity|
    buf << %Q[    ripper_id_#{id} = rb_intern_const("on_#{id}");\n]
  end
  buf << %Q[}\n]
  buf << %Q[\n]
  buf << %Q[static void\n]
  buf << %Q[ripper_init_eventids1_table(VALUE self)\n]
  buf << %Q[{\n]
  buf << %Q[    VALUE h = rb_hash_new();\n]
  buf << %Q[    ID id;\n]
  buf << %Q[    rb_define_const(self, "PARSER_EVENT_TABLE", h);\n]
  ids.each do |id, arity|
    buf << %Q[    id = rb_intern_const("#{id}");\n]
    buf << %Q[    rb_hash_aset(h, ID2SYM(id), INT2NUM(#{arity}));\n]
  end
  buf << %Q[}\n]
  buf
end

def generate_eventids2_table(ids)
  buf = ""
  buf << %Q[static void\n]
  buf << %Q[ripper_init_eventids2_table(VALUE self)\n]
  buf << %Q[{\n]
  buf << %Q[    VALUE h = rb_hash_new();\n]
  buf << %Q[    ID id;\n]
  buf << %Q[    rb_define_const(self, "SCANNER_EVENT_TABLE", h);\n]
  ids.each do |id|
    buf << %Q[    id = rb_intern_const("#{id}");\n]
    buf << %Q[    rb_hash_aset(h, ID2SYM(id), INT2NUM(1));\n]
  end
  buf << %Q[}\n]
  buf
end

def read_ids1(path)
  strip_locations(read_ids1_with_locations(path))
end

def strip_locations(h)
  h.map {|event, list| [event, list.first[1]] }\
      .sort_by {|event, arity| event.to_s }
end

def check_arity(h)
  invalid = false
  h.each do |event, list|
    unless list.map {|line, arity| arity }.uniq.size == 1
      invalid = true
      locations = list.map {|line, a| "#{line}:#{a}" }.join(', ')
      $stderr.puts "arity crash [event=#{event}]: #{locations}"
    end
  end
  abort if invalid
end

def read_ids1_with_locations(path)
  h = {}
  File.open(path) {|f|
    f.each do |line|
      next if /\A\#\s*define\s+s?dispatch/ =~ line
      next if /ripper_dispatch/ =~ line
      line.scan(/dispatch(\d)\((\w+)/) do |arity, event|
        (h[event] ||= []).push [f.lineno, arity.to_i]
      end
    end
  }
  h
end

def read_ids2(path)
  File.open(path) {|f|
    return f.read.scan(/ripper_id_(\w+)/).flatten.uniq.sort
  }
end

main
