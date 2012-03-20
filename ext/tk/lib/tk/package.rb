#
# tk/package.rb : package command
#
require 'tk'

module TkPackage
  include TkCore
  extend TkPackage

  TkCommandNames = ['package'.freeze].freeze

  def add_path(path)
    Tk::AUTO_PATH.value = Tk::AUTO_PATH.to_a << path
  end

  def forget(package)
    tk_call('package', 'forget', package)
    nil
  end

  def if_needed(pkg, ver, *arg, &b)
    size = arg.size

    if size==0 && !b
      # proc info
      procedure(tk_call('package', 'ifneeded', pkg, ver))

    elsif size==0 && b
      # set proc
      cmd = proc(&b)
      tk_call('package', 'ifneeded', pkg, ver, cmd)
      cmd

    elsif size==1 && !b
      # set proc
      cmd = arg[0]
      if cmd
        tk_call('package', 'ifneeded', pkg, ver, cmd)
        cmd
      else
        # remove proc
        tk_call('package', 'ifneeded', pkg, ver, '')
        nil
      end

    else
      fail ArgumentError, 'too many arguments'
    end
  end

  def names
    tk_split_simplelist(tk_call('package', 'names'))
  end

  def provide(package, version=nil)
    if version
      tk_call('package', 'provide', package, version)
    end
    if (ret = tk_call('package', 'provide', package)) == ''
      nil
    else
      ret
    end
  end

  def present(package, version=None)
    begin
      tk_call('package', 'present', package, version)
    rescue => e
      fail e.class, 'TkPackage ' << e.message
    end
  end

  def present_exact(package, version)
    begin
      tk_call('package', 'present', '-exact', package, version)
    rescue => e
      fail e.class, 'TkPackage ' << e.message
    end
  end

  def require(package, version=None)
    begin
      tk_call('package', 'require', package, version)
    rescue => e
      fail e.class, 'TkPackage ' << e.message
    end
  end

  def require_exact(package, version)
    begin
      tk_call('package', 'require', '-exact', package, version)
    rescue => e
      fail e.class, 'TkPackage ' << e.message
    end
  end

  def unknown_proc(*arg, &b)
    size = arg.size

    if size==0 && !b
      # proc info
      procedure(tk_call('package', 'unknown'))

    elsif size==0 && b
      # set proc
      cmd = proc(&b)
      tk_call('package', 'unknown', cmd)
      cmd

    elsif size==1 && !b
      # set proc
      cmd = arg[0]
      if cmd
        tk_call('package', 'unknown', cmd)
        cmd
      else
        # remove proc
        tk_call('package', 'unknown', '')
        nil
      end

    else
      fail ArgumentError, 'too many arguments'
    end
  end

  def versions(package)
    tk_split_simplelist(tk_call('package', 'versions', package))
  end

  def vcompare(version1, version2)
    number(tk_call('package', 'vcompare', version1, version2))
  end

  def vsatisfies(version1, version2)
    bool(tk_call('package', 'vsatisfies', version1, version2))
  end

  def prefer(setting = None)
    tk_call('package', 'prefer', setting)
  end
end
