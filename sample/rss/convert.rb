#!/usr/bin/env ruby

require "rss"

feeds = []
verbose = false
encoding = "UTF-8"
to_version = "1.0"

def error(exception)
  mark = "=" * 20
  mark = "#{mark} error #{mark}"
  STDERR.puts mark
  STDERR.puts exception.class
  STDERR.puts exception.message
  STDERR.puts exception.backtrace
  STDERR.puts mark
end

before_time = Time.now
ARGV.each do |fname|
  case fname
  when '-v'
    verbose = true
    next
  when /^-t(0\.91|1\.0|2\.0|atom)$/
    to_version = $1
    next
  end
  rss = nil
  f = File.read(fname)
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
    STDERR.puts "#{fname} does not include RSS 1.0 or 0.9x/2.0"
  else
    begin
      rss.output_encoding = encoding
    rescue RSS::UnknownConversionMethodError
      error($!) if verbose
    end
    feeds << [fname, rss]
  end
end
processing_time = Time.now - before_time

feeds.each do |fname, rss|
  converted_rss = rss.to_xml(to_version)
  output_name = fname.sub(/(\.[^\.]+)$/, "-#{to_version}\\1")
  File.open(output_name, "w") do |output|
    output.print(converted_rss)
  end
end

STDERR.puts "Used XML parser: #{RSS::Parser.default_parser}"
STDERR.puts "Processing time: #{processing_time}s"
