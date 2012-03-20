#! /usr/bin/ruby
# contributed by Minero Aoki.

require 'optparse'

parser = OptionParser.new
parser.on('-i') { puts "-i" }
parser.on('-o') { puts '-o' }

subparsers = Hash.new {|h,k|
  $stderr.puts "no such subcommand: #{k}"
  exit 1
}
subparsers['add'] = OptionParser.new.on('-i') { puts "add -i" }
subparsers['del'] = OptionParser.new.on('-i') { puts "del -i" }
subparsers['list'] = OptionParser.new.on('-i') { puts "list -i" }

parser.order!(ARGV)
subparsers[ARGV.shift].parse!(ARGV) unless ARGV.empty?
