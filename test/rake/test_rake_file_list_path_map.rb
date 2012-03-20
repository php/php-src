require File.expand_path('../helper', __FILE__)

class TestRakeFileListPathMap < Rake::TestCase
  def test_file_list_supports_pathmap
    assert_equal ['a', 'b'], FileList['dir/a.rb', 'dir/b.rb'].pathmap("%n")
  end
end

