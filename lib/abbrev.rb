#!/usr/bin/env ruby
#--
# Copyright (c) 2001,2003 Akinori MUSHA <knu@iDaemons.org>
#
# All rights reserved.  You can redistribute and/or modify it under
# the same terms as Ruby.
#
# $Idaemons: /home/cvs/rb/abbrev.rb,v 1.2 2001/05/30 09:37:45 knu Exp $
# $RoughId: abbrev.rb,v 1.4 2003/10/14 19:45:42 knu Exp $
# $Id$
#++

# Calculate the set of unique abbreviations for a given set of strings.
#
#   require 'abbrev'
#   require 'pp'
#
#   pp Abbrev::abbrev(['ruby', 'rules']).sort
#
# <i>Generates:</i>
#
#   [["rub", "ruby"],
#    ["ruby", "ruby"],
#    ["rul", "rules"],
#    ["rule", "rules"],
#    ["rules", "rules"]]
#
# Also adds an +abbrev+ method to class +Array+.

module Abbrev

  # Given a set of strings, calculate the set of unambiguous
  # abbreviations for those strings, and return a hash where the keys
  # are all the possible abbreviations and the values are the full
  # strings. Thus, given input of "car" and "cone", the keys pointing
  # to "car" would be "ca" and "car", while those pointing to "cone"
  # would be "co", "con", and "cone".
  #
  # The optional +pattern+ parameter is a pattern or a string. Only
  # those input strings matching the pattern, or begging the string,
  # are considered for inclusion in the output hash

  def abbrev(words, pattern = nil)
    table = {}
    seen = Hash.new(0)

    if pattern.is_a?(String)
      pattern = /^#{Regexp.quote(pattern)}/  # regard as a prefix
    end

    words.each do |word|
      next if (abbrev = word).empty?
      while (len = abbrev.rindex(/[\w\W]\z/)) > 0
        abbrev = word[0,len]

        next if pattern && pattern !~ abbrev

        case seen[abbrev] += 1
        when 1
          table[abbrev] = word
        when 2
          table.delete(abbrev)
        else
          break
        end
      end
    end

    words.each do |word|
      next if pattern && pattern !~ word

      table[word] = word
    end

    table
  end

  module_function :abbrev
end

class Array
  # Calculates the set of unambiguous abbreviations for the strings in
  # +self+. If passed a pattern or a string, only the strings matching
  # the pattern or starting with the string are considered.
  #
  #   %w{ car cone }.abbrev   #=> { "ca" => "car", "car" => "car",
  #                                 "co" => "cone", "con" => "cone",
  #                                 "cone" => "cone" }
  def abbrev(pattern = nil)
    Abbrev::abbrev(self, pattern)
  end
end

if $0 == __FILE__
  while line = gets
    hash = line.split.abbrev

    hash.sort.each do |k, v|
      puts "#{k} => #{v}"
    end
  end
end
