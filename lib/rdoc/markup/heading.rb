##
# A heading with a level (1-6) and text

class RDoc::Markup::Heading < Struct.new :level, :text

  ##
  # Calls #accept_heading on +visitor+

  def accept visitor
    visitor.accept_heading self
  end

  def pretty_print q # :nodoc:
    q.group 2, "[head: #{level} ", ']' do
      q.pp text
    end
  end

end

