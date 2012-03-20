require File.expand_path('../helper', __FILE__)

class TestRakeTopLevelFunctions < Rake::TestCase

  def setup
    super

    @app = Object.new

    def @app.called
      @called
    end

    def @app.method_missing(*a, &b)
      @called ||= []
      @called << [a, b]
      nil
    end

    Rake.application = @app
  end

  def test_namespace
    block = proc do end

    namespace("xyz", &block) 

    expected = [
      [[:in_namespace, 'xyz'], block]
    ]

    assert_equal expected, @app.called
  end

  def test_import
    import('x', 'y', 'z')

    expected = [
      [[:add_import, 'x'], nil],
      [[:add_import, 'y'], nil],
      [[:add_import, 'z'], nil],
    ]

    assert_equal expected, @app.called
  end

  def test_when_writing
    out, = capture_io do
      when_writing("NOTWRITING") do
        puts "WRITING"
      end
    end
    assert_equal "WRITING\n", out
  end

  def test_when_not_writing
    Rake::FileUtilsExt.nowrite_flag = true
    _, err = capture_io do
      when_writing("NOTWRITING") do
        puts "WRITING"
      end
    end
    assert_equal "DRYRUN: NOTWRITING\n", err
  ensure
    Rake::FileUtilsExt.nowrite_flag = false
  end

  def test_missing_constants_task
    Object.const_missing(:Task)

    expected = [
      [[:const_warning, :Task], nil]
    ]

    assert_equal expected, @app.called
  end

  def test_missing_constants_file_task
    Object.const_missing(:FileTask)

    expected = [
      [[:const_warning, :FileTask], nil]
    ]

    assert_equal expected, @app.called
  end

  def test_missing_constants_file_creation_task
    Object.const_missing(:FileCreationTask)

    expected = [
      [[:const_warning, :FileCreationTask], nil]
    ]

    assert_equal expected, @app.called
  end

  def test_missing_constants_rake_app
    Object.const_missing(:RakeApp)

    expected = [
      [[:const_warning, :RakeApp], nil]
    ]

    assert_equal expected, @app.called
  end

  def test_missing_other_constant
    assert_raises(NameError) do Object.const_missing(:Xyz) end
  end
end
