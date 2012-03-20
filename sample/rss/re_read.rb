#!/usr/bin/env ruby

require "rss"

def error(exception)
  mark = "=" * 20
  mark = "#{mark} error #{mark}"
  puts mark
  puts exception.class
  puts exception.message
  puts exception.backtrace
  puts mark
end

verbose = false
before_time = Time.now

ARGV.each do |fname|
  if fname == '-v'
    verbose = true
    next
  end
  source = nil
  File.open(fname) do |f|
    source = f.read
  end

  rss = nil
  read = false
  begin
    rss = RSS::Parser.parse(source)
    puts "Re-read valid feed: #{fname}"
    RSS::Parser.parse(rss.to_s)
    read = true
  rescue RSS::InvalidRSSError
    error($!) if verbose
    ## do non validate parse for invalid feed
    begin
      rss = RSS::Parser.parse(source, false)
    rescue RSS::Error
      ## invalid feed
      error($!) if verbose
    end
  rescue RSS::Error
    error($!) if verbose
  end

  if rss.nil?
    puts "Invalid feed: #{fname}"
  elsif !read
    puts "Re-read invalid feed: #{fname}"
    begin
      RSS::Parser.parse(rss.to_s)
    rescue RSS::Error
      puts "  Error occurred: #{fname}"
      error($!) if verbose
    end
  end
end

processing_time = Time.now - before_time

puts "Used XML parser: #{RSS::Parser.default_parser}"
puts "Processing time: #{processing_time}s"
