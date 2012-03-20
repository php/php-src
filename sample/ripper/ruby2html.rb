#!/usr/bin/env ruby
# $originalId: ruby2html.rb,v 1.2 2005/09/23 22:53:47 aamine Exp $

TEMPLATE_LINE = __LINE__ + 2
TEMPLATE = <<-EndTemplate
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=<%= encoding %>">
<% if css %>
  <link rel="stylesheet" type="text/css" href="<%= css %>">
<% end %>
  <title><%= File.basename(f.path) %></title>
</head>
<body>
<pre>
<%
    if print_line_number
      Ruby2HTML.compile(f).each_with_index do |line, idx|
%><%= sprintf('%4d  %s', idx+1, line) %><%
      end
    else
%><%= Ruby2HTML.compile(f) %><%
    end
%>
</pre>
</body>
</html>
EndTemplate

require 'ripper'
require 'stringio'
require 'cgi'
require 'erb'
require 'optparse'

def main
  encoding = 'us-ascii'
  css = nil
  print_line_number = false
  parser = OptionParser.new
  parser.banner = "Usage: #{File.basename($0)} [-l] [<file>...]"
  parser.on('--encoding=NAME', 'Character encoding [us-ascii].') {|name|
    encoding = name
  }
  parser.on('--css=URL', 'Set a link to CSS.') {|url|
    css = url
  }
  parser.on('-l', '--line-number', 'Show line number.') {
    print_line_number = true
  }
  parser.on('--help', 'Prints this message and quit.') {
    puts parser.help
    exit 0
  }
  begin
    parser.parse!
  rescue OptionParser::ParseError => err
    $stderr.puts err
    $stderr.puts parser.help
    exit 1
  end
  puts ruby2html(ARGF, encoding, css, print_line_number)
end

class ERB
  attr_accessor :lineno

  remove_method :result
  def result(b)
    eval(@src, b, (@filename || '(erb)'), (@lineno || 1))
  end
end

def ruby2html(f, encoding, css, print_line_number)
  erb = ERB.new(TEMPLATE, nil, '>')
  erb.filename = __FILE__
  erb.lineno = TEMPLATE_LINE
  erb.result(binding())
end

class Ruby2HTML < Ripper::Filter
  def Ruby2HTML.compile(f)
    buf = StringIO.new
    Ruby2HTML.new(f).parse(buf)
    buf.string
  end

  def on_default(event, tok, f)
    f << CGI.escapeHTML(tok)
  end

  def on_kw(tok, f)
    f << %Q[<span class="resword">#{CGI.escapeHTML(tok)}</span>]
  end

  def on_comment(tok, f)
    f << %Q[<span class="comment">#{CGI.escapeHTML(tok.rstrip)}</span>\n]
  end

  def on_tstring_beg(tok, f)
    f << %Q[<span class="string">#{CGI.escapeHTML(tok)}]
  end

  def on_tstring_end(tok, f)
    f << %Q[#{CGI.escapeHTML(tok)}</span>]
  end
end

if $0 == __FILE__
  main
end
