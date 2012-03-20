require File.expand_path('../helper', __FILE__)

class TestRakeWin32 < Rake::TestCase

  Win32 = Rake::Win32

  def test_win32_system_dir_uses_home_if_defined
    ENV['HOME'] = 'C:\\HP'

    assert_equal "C:/HP/Rake", Win32.win32_system_dir
  end

  def test_win32_system_dir_uses_homedrive_homepath_when_no_home_defined
    ENV['HOME']      = nil
    ENV['HOMEDRIVE'] = 'C:'
    ENV['HOMEPATH']  = '\\HP'

    assert_equal "C:/HP/Rake", Win32.win32_system_dir
  end

  def test_win32_system_dir_uses_appdata_when_no_home_or_home_combo
    ENV['APPDATA']   = "C:\\Documents and Settings\\HP\\Application Data"
    ENV['HOME']      = nil
    ENV['HOMEDRIVE'] = nil
    ENV['HOMEPATH']  = nil

    assert_equal "C:/Documents and Settings/HP/Application Data/Rake",
                 Win32.win32_system_dir
  end

  def test_win32_system_dir_fallback_to_userprofile_otherwise
    ENV['HOME']        = nil
    ENV['HOMEDRIVE']   = nil
    ENV['HOMEPATH']    = nil
    ENV['APPDATA']     = nil
    ENV['USERPROFILE'] = "C:\\Documents and Settings\\HP"

    assert_equal "C:/Documents and Settings/HP/Rake", Win32.win32_system_dir
  end

  def test_win32_system_dir_nil_of_no_env_vars
    ENV['APPDATA']     = nil
    ENV['HOME']        = nil
    ENV['HOMEDRIVE']   = nil
    ENV['HOMEPATH']    = nil
    ENV['RAKE_SYSTEM'] = nil
    ENV['USERPROFILE'] = nil

    assert_raises(Rake::Win32::Win32HomeError) do
      Win32.win32_system_dir
    end
  end

  def test_win32_backtrace_with_different_case
    ex = nil
    begin
     raise 'test exception'
    rescue => ex
    end

    ex.set_backtrace ['abc', 'rakefile']

    rake = Rake::Application.new
    rake.options.trace = true
    rake.instance_variable_set(:@rakefile, 'Rakefile')

    _, err = capture_io { rake.display_error_message(ex) }

    assert_match(/rakefile/, err)
  end

end
