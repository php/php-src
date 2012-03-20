#!/usr/bin/env ruby

require "nkf"
class String
  # From tdiary.rb
  def shorten( len = 120 )
    lines = NKF::nkf( "-t -m0 -f#{len}", self.gsub( /\n/, ' ' ) ).split( /\n/ )
    lines[0].concat( '...' ) if lines[0] and lines[1]
    lines[0]
  end
end

require "rss"

items = []
verbose = false

def error(exception)
  mark = "=" * 20
  mark = "#{mark} error #{mark}"
  puts mark
  puts exception.class
  puts exception.message
  puts exception.backtrace
  puts mark
end
before_time = Time.now
ARGV.each do |fname|
  if fname == '-v'
    verbose = true
    next
  end
  rss = nil
  f = File.new(fname).read
  begin
    ## do validate parse
    rss = RSS::Parser.parse(f)
  rescue RSS::InvalidRSSError
    error($!) if verbose
    ## do non validate parse for invalid RSS 1.0
    begin
      rss = RSS::Parser.parse(f, false)
    rescue RSS::Error
      ## invalid RSS.
      error($!) if verbose
    end
  rescue RSS::Error
    error($!) if verbose
  end
  if rss.nil?
    puts "#{fname} does not include RSS 1.0 or 0.9x/2.0"
  else
    begin
      rss.output_encoding = "utf-8"
    rescue RSS::UnknownConversionMethodError
      error($!) if verbose
    end

    rss = rss.to_rss("1.0") do |maker|
      maker.channel.about ||= maker.channel.link
      maker.channel.description ||= "No description"
      maker.items.each do |item|
        item.title ||= "UNKNOWN"
        item.link ||= "UNKNOWN"
      end
    end
    next if rss.nil?

    rss.items.each do |item|
      items << [rss.channel, item] if item.dc_date
    end
  end
end
processing_time = Time.now - before_time

items.sort do |x, y|
  y[1].dc_date <=> x[1].dc_date
end[0..20].each do |channel, item|
  puts "#{item.dc_date.localtime.iso8601}: " <<
    "#{channel.title}: #{item.title}"
  puts " Description: #{item.description.shorten(50)}" if item.description
end

puts "Used XML parser: #{RSS::Parser.default_parser}"
puts "Processing time: #{processing_time}s"
