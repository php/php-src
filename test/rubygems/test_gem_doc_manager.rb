require 'rubygems/test_case'
require 'rubygems/doc_manager'

class TestGemDocManager < Gem::TestCase

  def setup
    super

    @spec = quick_gem 'a', 2
    @manager = Gem::DocManager.new(@spec)
  end

  def test_uninstall_doc_unwritable
    path = @spec.base_dir
    orig_mode = File.stat(path).mode

    # File.chmod has no effect on MS Windows directories (it needs ACL).
    if win_platform?
      skip("test_uninstall_doc_unwritable skipped on MS Windows")
    else
      FileUtils.chmod 0000, path
    end

    assert_raises Gem::FilePermissionError do
      @manager.uninstall_doc
    end
  ensure
    FileUtils.chmod orig_mode, path
  end

end

