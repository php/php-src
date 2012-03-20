#!/usr/local/bin/ruby
# Illustration of a script to convert an RDoc-style file to a LaTeX document

require 'rdoc/markup'
require 'rdoc/markup/to_latex'

p = RDoc::Markup.new
h = RDoc::Markup::ToLaTeX.new

#puts "\\documentclass{report}"
#puts "\\usepackage{tabularx}"
#puts "\\usepackage{parskip}"
#puts "\\begin{document}"
puts p.convert(ARGF.read, h)
#puts "\\end{document}"
