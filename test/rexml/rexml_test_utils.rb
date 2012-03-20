module REXMLTestUtils
  def fixture_path(*components)
    File.join(File.dirname(__FILE__), "data", *components)
  end
end
