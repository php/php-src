#!/usr/bin/env ruby

# Creates the data structures needed by Onigurma to map Unicode codepoints to
# property names and POSIX character classes
#
# To use this, get UnicodeData.txt, Scripts.txt, PropList.txt,
# PropertyAliases.txt, PropertyValueAliases.txt, DerivedCoreProperties.txt,
# DerivedAge.txt and Blocks.txt  from unicode.org.
# (http://unicode.org/Public/UNIDATA/) And run following command.
# ruby1.9 tool/enc-unicode.rb data_dir > enc/unicode/name2ctype.kwd
# You can get source file for gperf.  After this, simply make ruby.

unless ARGV.size == 1
  $stderr.puts "Usage: #{$0} data_directory"
  exit(1)
end

POSIX_NAMES = %w[NEWLINE Alpha Blank Cntrl Digit Graph Lower Print Punct Space Upper XDigit Word Alnum ASCII]

def pair_codepoints(codepoints)

  # We have a sorted Array of codepoints that we wish to partition into
  # ranges such that the start- and endpoints form an inclusive set of
  # codepoints with property _property_. Note: It is intended that some ranges
  # will begin with the value with  which they end, e.g. 0x0020 -> 0x0020

  codepoints.sort!
  last_cp = codepoints.first
  pairs = [[last_cp, nil]]
  codepoints[1..-1].each do |codepoint|
    next if last_cp == codepoint

    # If the current codepoint does not follow directly on from the last
    # codepoint, the last codepoint represents the end of the current range,
    # and the current codepoint represents the start of the next range.
    if last_cp.next != codepoint
      pairs[-1][-1] = last_cp
      pairs << [codepoint, nil]
    end
    last_cp = codepoint
  end

  # The final pair has as its endpoint the last codepoint for this property
  pairs[-1][-1] = codepoints.last
  pairs
end

def parse_unicode_data(file)
  last_cp = 0
  data = {'Any' => (0x0000..0x10ffff).to_a, 'Assigned' => [],
    'ASCII' => (0..0x007F).to_a, 'NEWLINE' => [0x0a], 'Cn' => []}
  beg_cp = nil
  IO.foreach(file) do |line|
    fields = line.split(';')
    cp = fields[0].to_i(16)

    case fields[1]
    when /\A<(.*),\s*First>\z/
      beg_cp = cp
      next
    when /\A<(.*),\s*Last>\z/
      cps = (beg_cp..cp).to_a
    else
      beg_cp = cp
      cps = [cp]
    end

    # The Cn category represents unassigned characters. These are not listed in
    # UnicodeData.txt so we must derive them by looking for 'holes' in the range
    # of listed codepoints. We increment the last codepoint seen and compare it
    # with the current codepoint. If the current codepoint is less than
    # last_cp.next we have found a hole, so we add the missing codepoint to the
    # Cn category.
    data['Cn'].concat((last_cp.next...beg_cp).to_a)

    # Assigned - Defined in unicode.c; interpreted as every character in the
    # Unicode range minus the unassigned characters
    data['Assigned'].concat(cps)

    # The third field denotes the 'General' category, e.g. Lu
    (data[fields[2]] ||= []).concat(cps)

    # The 'Major' category is the first letter of the 'General' category, e.g.
    # 'Lu' -> 'L'
    (data[fields[2][0,1]] ||= []).concat(cps)
    last_cp = cp
  end

  # The last Cn codepoint should be 0x10ffff. If it's not, append the missing
  # codepoints to Cn and C
  cn_remainder = (last_cp.next..0x10ffff).to_a
  data['Cn'] += cn_remainder
  data['C'] += data['Cn']

  # Special case for LC (Cased_Letter). LC = Ll + Lt + Lu
  data['LC'] = data['Ll'] + data['Lt'] + data['Lu']

  # Define General Category properties
  gcps = data.keys.sort - POSIX_NAMES

  # Returns General Category Property names and the data
  [gcps, data]
end

def define_posix_props(data)
  # We now derive the character classes (POSIX brackets), e.g. [[:alpha:]]
  #

  data['Alpha'] = data['Alphabetic']
  data['Upper'] = data['Uppercase']
  data['Lower'] = data['Lowercase']
  data['Punct'] = data['Punctuation']
  data['Digit'] = data['Decimal_Number']
  data['XDigit'] = (0x0030..0x0039).to_a + (0x0041..0x0046).to_a +
                   (0x0061..0x0066).to_a
  data['Alnum'] = data['Alpha'] + data['Digit']
  data['Space'] = data['White_Space']
  data['Blank'] = data['Space_Separator'] + [0x0009]
  data['Cntrl'] = data['Cc']
  data['Word'] = data['Alpha'] + data['Mark'] + data['Digit'] + data['Connector_Punctuation']
  data['Graph'] = data['Any'] - data['Space'] - data['Cntrl'] -
    data['Surrogate'] - data['Unassigned']
  data['Print'] = data['Graph'] + data['Space_Separator']
end

def parse_scripts(data, categories)
  files = [
    {:fn => 'DerivedCoreProperties.txt', :title => 'Derived Property'},
    {:fn => 'Scripts.txt', :title => 'Script'},
    {:fn => 'PropList.txt', :title => 'Binary Property'}
  ]
  current = nil
  cps = []
  names = {}
  files.each do |file|
    IO.foreach(get_file(file[:fn])) do |line|
      if /^# Total code points: / =~ line
        data[current] = cps
        categories[current] = file[:title]
        (names[file[:title]] ||= []) << current
        cps = []
      elsif /^([0-9a-fA-F]+)(?:..([0-9a-fA-F]+))?\s*;\s*(\w+)/ =~ line
        current = $3
        $2 ? cps.concat(($1.to_i(16)..$2.to_i(16)).to_a) : cps.push($1.to_i(16))
      end
    end
  end
  #  All code points not explicitly listed for Script
  #  have the value Unknown (Zzzz).
  data['Unknown'] =  (0..0x10ffff).to_a - data.values_at(*names['Script']).flatten
  categories['Unknown'] = 'Script'
  names.values.flatten << 'Unknown'
end

def parse_aliases(data)
  kv = {}
  IO.foreach(get_file('PropertyAliases.txt')) do |line|
    next unless /^(\w+)\s*; (\w+)/ =~ line
    data[$1] = data[$2]
    kv[normalize_propname($1)] = normalize_propname($2)
  end
  IO.foreach(get_file('PropertyValueAliases.txt')) do |line|
    next unless /^(sc|gc)\s*; (\w+)\s*; (\w+)(?:\s*; (\w+))?/ =~ line
    if $1 == 'gc'
      data[$3] = data[$2]
      data[$4] = data[$2]
      kv[normalize_propname($3)] = normalize_propname($2)
      kv[normalize_propname($4)] = normalize_propname($2) if $4
    else
      data[$2] = data[$3]
      data[$4] = data[$3]
      kv[normalize_propname($2)] = normalize_propname($3)
      kv[normalize_propname($4)] = normalize_propname($3) if $4
    end
  end
  kv
end

# According to Unicode6.0.0/ch03.pdf, Section 3.1, "An update version
# never involves any additions to the character repertoire." Versions
# in DerivedAge.txt should always be /\d+\.\d+/
def parse_age(data)
  current = nil
  last_constname = nil
  cps = []
  ages = []
  IO.foreach(get_file('DerivedAge.txt')) do |line|
    if /^# Total code points: / =~ line
      constname = constantize_agename(current)
			# each version matches all previous versions
      cps.concat(data[last_constname]) if last_constname
      data[constname] = cps
      make_const(constname, cps, "Derived Age #{current}")
      ages << current
      last_constname = constname
      cps = []
    elsif /^([0-9a-fA-F]+)(?:..([0-9a-fA-F]+))?\s*;\s*(\d+\.\d+)/ =~ line
      current = $3
      $2 ? cps.concat(($1.to_i(16)..$2.to_i(16)).to_a) : cps.push($1.to_i(16))
    end
  end
  ages
end

def parse_block(data)
  current = nil
  last_constname = nil
  cps = []
  blocks = []
  IO.foreach(get_file('Blocks.txt')) do |line|
    if /^([0-9a-fA-F]+)\.\.([0-9a-fA-F]+);\s*(.*)/ =~ line
      cps = ($1.to_i(16)..$2.to_i(16)).to_a
      constname = constantize_blockname($3)
      data[constname] = cps
      make_const(constname, cps, "Block")
      blocks << constname
    end
  end

  # All code points not belonging to any of the named blocks
  # have the value No_Block.
  no_block = (0..0x10ffff).to_a - data.values_at(*blocks).flatten
  constname = constantize_blockname("No_Block")
  make_const(constname, no_block, "Block")
  blocks << constname
end

$const_cache = {}
# make_const(property, pairs, name): Prints a 'static const' structure for a
# given property, group of paired codepoints, and a human-friendly name for
# the group
def make_const(prop, data, name)
  puts "\n/* '#{prop}': #{name} */"
  if origprop = $const_cache.index(data)
    puts "#define CR_#{prop} CR_#{origprop}"
  else
    $const_cache[prop] = data
    pairs = pair_codepoints(data)
    puts "static const OnigCodePoint CR_#{prop}[] = {"
    # The first element of the constant is the number of pairs of codepoints
    puts "\t#{pairs.size},"
    pairs.each do |pair|
      pair.map! { |c|  c == 0 ? '0x0000' : sprintf("%0#6x", c) }
      puts "\t#{pair.first}, #{pair.last},"
    end
    puts "}; /* CR_#{prop} */"
  end
end

def normalize_propname(name)
  name = name.downcase
  name.delete!('- _')
  name
end

def constantize_agename(name)
  "Age_#{name.sub(/\./, '_')}"
end

def constantize_blockname(name)
  "In_#{name.gsub(/\W/, '_')}"
end

def get_file(name)
  File.join(ARGV[0], name)
end


# Write Data
puts '%{'
puts '#define long size_t'
props, data = parse_unicode_data(get_file('UnicodeData.txt'))
categories = {}
props.concat parse_scripts(data, categories)
aliases = parse_aliases(data)
define_posix_props(data)
POSIX_NAMES.each do |name|
  make_const(name, data[name], "[[:#{name}:]]")
end
print "\n#ifdef USE_UNICODE_PROPERTIES"
props.each do |name|
  category = categories[name] ||
    case name.size
    when 1 then 'Major Category'
    when 2 then 'General Category'
    else        '-'
    end
  make_const(name, data[name], category)
end
ages = parse_age(data)
blocks = parse_block(data)
puts '#endif /* USE_UNICODE_PROPERTIES */'
puts(<<'__HEREDOC')

static const OnigCodePoint* const CodeRanges[] = {
__HEREDOC
POSIX_NAMES.each{|name|puts"  CR_#{name},"}
puts "#ifdef USE_UNICODE_PROPERTIES"
props.each{|name| puts"  CR_#{name},"}
ages.each{|name|  puts"  CR_#{constantize_agename(name)},"}
blocks.each{|name|puts"  CR_#{name},"}

puts(<<'__HEREDOC')
#endif /* USE_UNICODE_PROPERTIES */
};
struct uniname2ctype_struct {
  int name, ctype;
};

static const struct uniname2ctype_struct *uniname2ctype_p(const char *, unsigned int);
%}
struct uniname2ctype_struct;
%%
__HEREDOC
i = -1
name_to_index = {}
POSIX_NAMES.each do |name|
  i += 1
  next if name == 'NEWLINE'
  name = normalize_propname(name)
  name_to_index[name] = i
  puts"%-40s %3d" % [name + ',', i]
end
puts "#ifdef USE_UNICODE_PROPERTIES"
props.each do |name|
  i += 1
  name = normalize_propname(name)
  name_to_index[name] = i
  puts "%-40s %3d" % [name + ',', i]
end
aliases.each_pair do |k, v|
  next if name_to_index[k]
  next unless v = name_to_index[v]
  puts "%-40s %3d" % [k + ',', v]
end
ages.each do |name|
  i += 1
  name = "age=#{name}"
  name_to_index[name] = i
  puts "%-40s %3d" % [name + ',', i]
end
blocks.each do |name|
  i += 1
  name = normalize_propname(name)
  name_to_index[name] = i
  puts "%-40s %3d" % [name + ',', i]
end
puts(<<'__HEREDOC')
#endif /* USE_UNICODE_PROPERTIES */
%%
static int
uniname2ctype(const UChar *name, unsigned int len)
{
  const struct uniname2ctype_struct *p = uniname2ctype_p((const char *)name, len);
  if (p) return p->ctype;
  return -1;
}
__HEREDOC
