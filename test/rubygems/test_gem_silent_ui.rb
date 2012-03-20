require 'rubygems/test_case'
require 'rubygems/user_interaction'
require 'timeout'

class TestGemSilentUI < Gem::TestCase

  def setup
    super
    @sui = Gem::SilentUI.new
  end

  def test_ask
    value = nil
    out, err = capture_io do
      use_ui @sui do
        value = @sui.ask 'Problem?'
      end
    end

    assert_empty out, 'No output'
    assert_empty err, 'No output'

    assert_nil value, 'No value'
  end

  def test_ask_for_password
    value = nil
    out, err = capture_io do
      use_ui @sui do
        value = @sui.ask_for_password 'Problem?'
      end
    end

    assert_empty out, 'No output'
    assert_empty err, 'No output'

    assert_nil value, 'No value'
  end

  def test_ask_yes_no
    value = nil
    out, err = capture_io do
      use_ui @sui do
        assert_raises(Gem::OperationNotSupportedError) do
          @sui.ask_yes_no 'Problem?'
        end
      end
    end

    assert_empty out, 'No output'
    assert_empty err, 'No output'
    
    out, err = capture_io do
      use_ui @sui do
        value = @sui.ask_yes_no 'Problem?', true
      end
    end

    assert_empty out, 'No output'
    assert_empty err, 'No output'

    assert value, 'Value is true'
    
    out, err = capture_io do
      use_ui @sui do
        value = @sui.ask_yes_no 'Problem?', false
      end
    end

    assert_empty out, 'No output'
    assert_empty err, 'No output'

    assert_equal value, false, 'Value is false'
  end

  def test_choose_from_list
    value = nil
    out, err = capture_io do
      use_ui @sui do
        value = @sui.choose_from_list 'Problem?', %w[yes no]
      end
    end

    assert_empty out, 'No output'
    assert_empty err, 'No output'

    assert_equal [nil, nil], value, 'Value is nil!'
  end

  def test_progress_reporter
    out, err = capture_io do
      use_ui @sui do
        @sui.progress_reporter 10, 'hi'
      end
    end

    assert_empty out, 'No output'
    assert_empty err, 'No output'
  end

  def test_download_reporter
    out, err = capture_io do
      use_ui @sui do
        @sui.download_reporter.fetch 'a.gem', 1024
      end
    end

    assert_empty out, 'No output'
    assert_empty err, 'No output'
  end
end
