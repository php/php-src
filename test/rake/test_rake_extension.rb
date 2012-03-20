require File.expand_path('../helper', __FILE__)
require 'stringio'

class TestRakeExtension < Rake::TestCase

  module Redirect
    def error_redirect
      old_err = $stderr
      result = StringIO.new
      $stderr = result
      yield
      result
    ensure
      $stderr = old_err
    end
  end

  class Sample
    extend Redirect

    def duplicate_method
      :original
    end

    OK_ERRS = error_redirect do
      rake_extension("a") do
        def ok_method
        end
      end
    end


    DUP_ERRS = error_redirect do
      rake_extension("duplicate_method") do
        def duplicate_method
          :override
        end
      end
    end
  end

  def test_methods_actually_exist
    sample = Sample.new
    sample.ok_method
    sample.duplicate_method
  end

  def test_no_warning_when_defining_ok_method
    assert_equal "", Sample::OK_ERRS.string
  end

  def test_extension_complains_when_a_method_that_is_present
    assert_match(/warning:/i, Sample::DUP_ERRS.string)
    assert_match(/already exists/i, Sample::DUP_ERRS.string)
    assert_match(/duplicate_method/i, Sample::DUP_ERRS.string)
    assert_equal :original, Sample.new.duplicate_method
  end

end
