##
# Parse a non-source file. We basically take the whole thing as one big
# comment.

class RDoc::Parser::Simple < RDoc::Parser

  parse_files_matching(//)

  attr_reader :content # :nodoc:

  ##
  # Prepare to parse a plain file

  def initialize(top_level, file_name, content, options, stats)
    super

    preprocess = RDoc::Markup::PreProcess.new @file_name, @options.rdoc_include

    preprocess.handle @content, @top_level
  end

  ##
  # Extract the file contents and attach them to the TopLevel as a comment

  def scan
    comment = remove_coding_comment @content
    comment = remove_private_comments comment

    @top_level.comment = comment
    @top_level.parser = self.class
    @top_level
  end

  ##
  # Removes comments wrapped in <tt>--/++</tt>

  def remove_private_comments text
    text.gsub(/^--\n.*?^\+\+/m, '').sub(/^--\n.*/m, '')
  end

  ##
  # Removes the encoding magic comment from +text+

  def remove_coding_comment text
    text.sub(/\A# .*coding[=:].*$/, '')
  end

end

