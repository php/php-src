# -*- ruby -*-

require 'optparse'
require 'uri'

OptionParser.accept(URI) {|s,| URI.parse(s) if s}
