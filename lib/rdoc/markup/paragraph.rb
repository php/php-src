##
# A Paragraph of text

class RDoc::Markup::Paragraph < RDoc::Markup::Raw

  ##
  # Calls #accept_paragraph on +visitor+

  def accept visitor
    visitor.accept_paragraph self
  end

end

