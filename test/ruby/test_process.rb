require 'test/unit'
require 'tmpdir'
require 'pathname'
require 'timeout'
require_relative 'envutil'
require 'rbconfig'

class TestProcess < Test::Unit::TestCase
  RUBY = EnvUtil.rubybin

  def setup
    Process.waitall
  end

  def teardown
    Process.waitall
  end

  def windows?
    return /mswin|mingw|bccwin/ =~ RUBY_PLATFORM
  end

  def write_file(filename, content)
    File.open(filename, "w") {|f|
      f << content
    }
  end

  def with_tmpchdir
    Dir.mktmpdir {|d|
      d = Pathname.new(d).realpath.to_s
      Dir.chdir(d) {
        yield d
      }
    }
  end

  def run_in_child(str) # should be called in a temporary directory
    write_file("test-script", str)
    Process.wait spawn(RUBY, "test-script")
    $?
  end

  def test_rlimit_availability
    begin
      Process.getrlimit(nil)
    rescue NotImplementedError
      assert_raise(NotImplementedError) { Process.setrlimit }
    rescue TypeError
      assert_raise(ArgumentError) { Process.setrlimit }
    end
  end

  def rlimit_exist?
    Process.getrlimit(nil)
  rescue NotImplementedError
    return false
  rescue TypeError
    return true
  end

  def test_rlimit_nofile
    return unless rlimit_exist?
    with_tmpchdir {
      write_file 's', <<-"End"
	# if limit=0, this test freeze pn OpenBSD
	limit = /openbsd/ =~ RUBY_PLATFORM ? 1 : 0
	result = 1
	begin
	  Process.setrlimit(Process::RLIMIT_NOFILE, limit)
	rescue Errno::EINVAL
	  result = 0
	end
	if result == 1
	  begin
	    IO.pipe
	  rescue Errno::EMFILE
	   result = 0
	  end
	end
	exit result
      End
      pid = spawn RUBY, "s"
      Process.wait pid
      assert_equal(0, $?.to_i, "#{$?}")
    }
  end

  def test_rlimit_name
    return unless rlimit_exist?
    [
      :AS, "AS",
      :CORE, "CORE",
      :CPU, "CPU",
      :DATA, "DATA",
      :FSIZE, "FSIZE",
      :MEMLOCK, "MEMLOCK",
      :MSGQUEUE, "MSGQUEUE",
      :NICE, "NICE",
      :NOFILE, "NOFILE",
      :NPROC, "NPROC",
      :RSS, "RSS",
      :RTPRIO, "RTPRIO",
      :RTTIME, "RTTIME",
      :SBSIZE, "SBSIZE",
      :SIGPENDING, "SIGPENDING",
      :STACK, "STACK",
    ].each {|name|
      if Process.const_defined? "RLIMIT_#{name}"
        assert_nothing_raised { Process.getrlimit(name) }
      else
        assert_raise(ArgumentError) { Process.getrlimit(name) }
      end
    }
    assert_raise(ArgumentError) { Process.getrlimit(:FOO) }
    assert_raise(ArgumentError) { Process.getrlimit("FOO") }
  end

  def test_rlimit_value
    return unless rlimit_exist?
    assert_raise(ArgumentError) { Process.setrlimit(:CORE, :FOO) }
    with_tmpchdir do
      s = run_in_child(<<-'End')
        cur, max = Process.getrlimit(:NOFILE)
        Process.setrlimit(:NOFILE, [max-10, cur].min)
        begin
          Process.setrlimit(:NOFILE, :INFINITY)
        rescue Errno::EPERM
          exit false
        end
      End
      assert_not_equal(0, s.exitstatus)
      s = run_in_child(<<-'End')
        cur, max = Process.getrlimit(:NOFILE)
        Process.setrlimit(:NOFILE, [max-10, cur].min)
        begin
          Process.setrlimit(:NOFILE, "INFINITY")
        rescue Errno::EPERM
          exit false
        end
      End
      assert_not_equal(0, s.exitstatus)
    end
  end

  TRUECOMMAND = [RUBY, '-e', '']

  def test_execopts_opts
    assert_nothing_raised {
      Process.wait Process.spawn(*TRUECOMMAND, {})
    }
    assert_raise(ArgumentError) {
      Process.wait Process.spawn(*TRUECOMMAND, :foo => 100)
    }
    assert_raise(ArgumentError) {
      Process.wait Process.spawn(*TRUECOMMAND, Process => 100)
    }
  end

  def test_execopts_pgroup
    skip "system(:pgroup) is not supported" if windows?
    assert_nothing_raised { system(*TRUECOMMAND, :pgroup=>false) }

    io = IO.popen([RUBY, "-e", "print Process.getpgrp"])
    assert_equal(Process.getpgrp.to_s, io.read)
    io.close

    io = IO.popen([RUBY, "-e", "print Process.getpgrp", :pgroup=>true])
    assert_equal(io.pid.to_s, io.read)
    io.close

    assert_raise(ArgumentError) { system(*TRUECOMMAND, :pgroup=>-1) }
    assert_raise(Errno::EPERM) { Process.wait spawn(*TRUECOMMAND, :pgroup=>2) }

    io1 = IO.popen([RUBY, "-e", "print Process.getpgrp", :pgroup=>true])
    io2 = IO.popen([RUBY, "-e", "print Process.getpgrp", :pgroup=>io1.pid])
    assert_equal(io1.pid.to_s, io1.read)
    assert_equal(io1.pid.to_s, io2.read)
    Process.wait io1.pid
    Process.wait io2.pid
    io1.close
    io2.close
  end

  def test_execopts_rlimit
    return unless rlimit_exist?
    assert_raise(ArgumentError) { system(*TRUECOMMAND, :rlimit_foo=>0) }
    assert_raise(ArgumentError) { system(*TRUECOMMAND, :rlimit_NOFILE=>0) }
    assert_raise(ArgumentError) { system(*TRUECOMMAND, :rlimit_nofile=>[]) }
    assert_raise(ArgumentError) { system(*TRUECOMMAND, :rlimit_nofile=>[1,2,3]) }

    max = Process.getrlimit(:CORE).last

    n = max
    IO.popen([RUBY, "-e",
             "p Process.getrlimit(:CORE)", :rlimit_core=>n]) {|io|
      assert_equal("[#{n}, #{n}]\n", io.read)
    }

    n = 0
    IO.popen([RUBY, "-e",
             "p Process.getrlimit(:CORE)", :rlimit_core=>n]) {|io|
      assert_equal("[#{n}, #{n}]\n", io.read)
    }

    n = max
    IO.popen([RUBY, "-e",
             "p Process.getrlimit(:CORE)", :rlimit_core=>[n]]) {|io|
      assert_equal("[#{n}, #{n}]", io.read.chomp)
    }

    m, n = 0, max
    IO.popen([RUBY, "-e",
             "p Process.getrlimit(:CORE)", :rlimit_core=>[m,n]]) {|io|
      assert_equal("[#{m}, #{n}]", io.read.chomp)
    }

    m, n = 0, 0
    IO.popen([RUBY, "-e",
             "p Process.getrlimit(:CORE)", :rlimit_core=>[m,n]]) {|io|
      assert_equal("[#{m}, #{n}]", io.read.chomp)
    }

    n = max
    IO.popen([RUBY, "-e",
      "p Process.getrlimit(:CORE), Process.getrlimit(:CPU)",
      :rlimit_core=>n, :rlimit_cpu=>3600]) {|io|
      assert_equal("[#{n}, #{n}]\n[3600, 3600]", io.read.chomp)
    }
  end

  MANDATORY_ENVS = %w[RUBYLIB]
  case RbConfig::CONFIG['target_os']
  when /linux/
    MANDATORY_ENVS << 'LD_PRELOAD'
  when /mswin|mingw/
    MANDATORY_ENVS.concat(%w[HOME USER TMPDIR])
  end
  if e = RbConfig::CONFIG['LIBPATHENV']
    MANDATORY_ENVS << e
  end
  PREENVARG = ['-e', "%w[#{MANDATORY_ENVS.join(' ')}].each{|e|ENV.delete(e)}"]
  ENVARG = ['-e', 'ENV.each {|k,v| puts "#{k}=#{v}" }']
  ENVCOMMAND = [RUBY].concat(PREENVARG).concat(ENVARG)

  def test_execopts_env
    assert_raise(ArgumentError) {
      system({"F=O"=>"BAR"}, *TRUECOMMAND)
    }

    with_tmpchdir {|d|
      prog = "#{d}/notexist"
      e = assert_raise(Errno::ENOENT) {
        Process.wait Process.spawn({"FOO"=>"BAR"}, prog)
      }
      assert_equal(prog, e.message.sub(/.* - /, ''))
      e = assert_raise(Errno::ENOENT) {
        Process.wait Process.spawn({"FOO"=>"BAR"}, [prog, "blar"])
      }
      assert_equal(prog, e.message.sub(/.* - /, ''))
    }
    h = {}
    cmd = [h, RUBY]
    (ENV.keys + MANDATORY_ENVS).each do |k|
      case k
      when /\APATH\z/i
      when *MANDATORY_ENVS
        cmd << '-e' << "ENV.delete('#{k}')"
      else
        h[k] = nil
      end
    end
    cmd << '-e' << 'puts ENV.keys.map{|e|e.upcase}'
    IO.popen(cmd) {|io|
      assert_equal("PATH\n", io.read)
    }

    IO.popen([{"FOO"=>"BAR"}, *ENVCOMMAND]) {|io|
      assert_match(/^FOO=BAR$/, io.read)
    }

    with_tmpchdir {|d|
      system({"fofo"=>"haha"}, *ENVCOMMAND, STDOUT=>"out")
      assert_match(/^fofo=haha$/, File.read("out").chomp)
    }

    old = ENV["hmm"]
    begin
      ENV["hmm"] = "fufu"
      IO.popen(ENVCOMMAND) {|io| assert_match(/^hmm=fufu$/, io.read) }
      IO.popen([{"hmm"=>""}, *ENVCOMMAND]) {|io| assert_match(/^hmm=$/, io.read) }
      IO.popen([{"hmm"=>nil}, *ENVCOMMAND]) {|io| assert_not_match(/^hmm=/, io.read) }
      ENV["hmm"] = ""
      IO.popen(ENVCOMMAND) {|io| assert_match(/^hmm=$/, io.read) }
      IO.popen([{"hmm"=>""}, *ENVCOMMAND]) {|io| assert_match(/^hmm=$/, io.read) }
      IO.popen([{"hmm"=>nil}, *ENVCOMMAND]) {|io| assert_not_match(/^hmm=/, io.read) }
      ENV["hmm"] = nil
      IO.popen(ENVCOMMAND) {|io| assert_not_match(/^hmm=/, io.read) }
      IO.popen([{"hmm"=>""}, *ENVCOMMAND]) {|io| assert_match(/^hmm=$/, io.read) }
      IO.popen([{"hmm"=>nil}, *ENVCOMMAND]) {|io| assert_not_match(/^hmm=/, io.read) }
    ensure
      ENV["hmm"] = old
    end
  end

  def test_execopts_unsetenv_others
    h = {}
    MANDATORY_ENVS.each {|k| e = ENV[k] and h[k] = e}
    IO.popen([h, *ENVCOMMAND, :unsetenv_others=>true]) {|io|
      assert_equal("", io.read)
    }
    IO.popen([h.merge("A"=>"B"), *ENVCOMMAND, :unsetenv_others=>true]) {|io|
      assert_equal("A=B\n", io.read)
    }
  end

  PWD = [RUBY, '-e', 'puts Dir.pwd']

  def test_execopts_chdir
    with_tmpchdir {|d|
      IO.popen([*PWD, :chdir => d]) {|io|
        assert_equal(d, io.read.chomp)
      }
      assert_raise(Errno::ENOENT) {
        Process.wait Process.spawn(*PWD, :chdir => "d/notexist")
      }
    }
  end

  def test_execopts_open_chdir
    with_tmpchdir {|d|
      Dir.mkdir "foo"
      system(*PWD, :chdir => "foo", :out => "open_chdir_test")
      assert(File.exist?("open_chdir_test"))
      assert(!File.exist?("foo/open_chdir_test"))
      assert_equal("#{d}/foo", File.read("open_chdir_test").chomp)
    }
  end

  UMASK = [RUBY, '-e', 'printf "%04o\n", File.umask']

  def test_execopts_umask
    skip "umask is not supported" if windows?
    IO.popen([*UMASK, :umask => 0]) {|io|
      assert_equal("0000", io.read.chomp)
    }
    IO.popen([*UMASK, :umask => 0777]) {|io|
      assert_equal("0777", io.read.chomp)
    }
  end

  def with_pipe
    begin
      r, w = IO.pipe
      yield r, w
    ensure
      r.close unless r.closed?
      w.close unless w.closed?
    end
  end

  def with_pipes(n)
    ary = []
    begin
      n.times {
        ary << IO.pipe
      }
      yield ary
    ensure
      ary.each {|r, w|
        r.close unless r.closed?
        w.close unless w.closed?
      }
    end
  end

  ECHO = lambda {|arg| [RUBY, '-e', "puts #{arg.dump}; STDOUT.flush"] }
  SORT = [RUBY, '-e', "puts ARGF.readlines.sort"]
  CAT = [RUBY, '-e', "IO.copy_stream STDIN, STDOUT"]

  def test_execopts_redirect
    with_tmpchdir {|d|
      Process.wait Process.spawn(*ECHO["a"], STDOUT=>["out", File::WRONLY|File::CREAT|File::TRUNC, 0644])
      assert_equal("a", File.read("out").chomp)
      if windows?
        # currently telling to child the file modes is not supported.
        open("out", "a") {|f| f.write "0\n"}
      else
        Process.wait Process.spawn(*ECHO["0"], STDOUT=>["out", File::WRONLY|File::CREAT|File::APPEND, 0644])
        assert_equal("a\n0\n", File.read("out"))
      end
      Process.wait Process.spawn(*SORT, STDIN=>["out", File::RDONLY, 0644],
                                         STDOUT=>["out2", File::WRONLY|File::CREAT|File::TRUNC, 0644])
      assert_equal("0\na\n", File.read("out2"))
      Process.wait Process.spawn(*ECHO["b"], [STDOUT, STDERR]=>["out", File::WRONLY|File::CREAT|File::TRUNC, 0644])
      assert_equal("b", File.read("out").chomp)
      # problem occur with valgrind
      #Process.wait Process.spawn(*ECHO["a"], STDOUT=>:close, STDERR=>["out", File::WRONLY|File::CREAT|File::TRUNC, 0644])
      #p File.read("out")
      #assert(!File.read("out").empty?) # error message such as "-e:1:in `flush': Bad file descriptor (Errno::EBADF)"
      Process.wait Process.spawn(*ECHO["c"], STDERR=>STDOUT, STDOUT=>["out", File::WRONLY|File::CREAT|File::TRUNC, 0644])
      assert_equal("c", File.read("out").chomp)
      File.open("out", "w") {|f|
        Process.wait Process.spawn(*ECHO["d"], STDOUT=>f)
        assert_equal("d", File.read("out").chomp)
      }
      opts = {STDOUT=>["out", File::WRONLY|File::CREAT|File::TRUNC, 0644]}
      opts.merge(3=>STDOUT, 4=>STDOUT, 5=>STDOUT, 6=>STDOUT, 7=>STDOUT) unless windows?
      Process.wait Process.spawn(*ECHO["e"], opts)
      assert_equal("e", File.read("out").chomp)
      opts = {STDOUT=>["out", File::WRONLY|File::CREAT|File::TRUNC, 0644]}
      opts.merge(3=>0, 4=>:in, 5=>STDIN, 6=>1, 7=>:out, 8=>STDOUT, 9=>2, 10=>:err, 11=>STDERR) unless windows?
      Process.wait Process.spawn(*ECHO["ee"], opts)
      assert_equal("ee", File.read("out").chomp)
      unless windows?
        # passing non-stdio fds is not supported on Windows
        File.open("out", "w") {|f|
          h = {STDOUT=>f, f=>STDOUT}
          3.upto(30) {|i| h[i] = STDOUT if f.fileno != i }
          Process.wait Process.spawn(*ECHO["f"], h)
          assert_equal("f", File.read("out").chomp)
        }
      end
      assert_raise(ArgumentError) {
        Process.wait Process.spawn(*ECHO["f"], 1=>Process)
      }
      assert_raise(ArgumentError) {
        Process.wait Process.spawn(*ECHO["f"], [Process]=>1)
      }
      assert_raise(ArgumentError) {
        Process.wait Process.spawn(*ECHO["f"], [1, STDOUT]=>2)
      }
      assert_raise(ArgumentError) {
        Process.wait Process.spawn(*ECHO["f"], -1=>2)
      }
      Process.wait Process.spawn(*ECHO["hhh\nggg\n"], STDOUT=>"out")
      assert_equal("hhh\nggg\n", File.read("out"))
      Process.wait Process.spawn(*SORT, STDIN=>"out", STDOUT=>"out2")
      assert_equal("ggg\nhhh\n", File.read("out2"))

      unless windows?
        # passing non-stdio fds is not supported on Windows
        assert_raise(Errno::ENOENT) {
          Process.wait Process.spawn("non-existing-command", (3..60).to_a=>["err", File::WRONLY|File::CREAT])
        }
        assert_equal("", File.read("err"))
      end

      system(*ECHO["bb\naa\n"], STDOUT=>["out", "w"])
      assert_equal("bb\naa\n", File.read("out"))
      system(*SORT, STDIN=>["out"], STDOUT=>"out2")
      assert_equal("aa\nbb\n", File.read("out2"))

      with_pipe {|r1, w1|
        with_pipe {|r2, w2|
          opts = {STDIN=>r1, STDOUT=>w2}
          opts.merge(w1=>:close, r2=>:close) unless windows?
          pid = spawn(*SORT, opts)
          r1.close
          w2.close
          w1.puts "c"
          w1.puts "a"
          w1.puts "b"
          w1.close
          assert_equal("a\nb\nc\n", r2.read)
          r2.close
          Process.wait(pid)
        }
      }

      unless windows?
        # passing non-stdio fds is not supported on Windows
        with_pipes(5) {|pipes|
          ios = pipes.flatten
          h = {}
          ios.length.times {|i| h[ios[i]] = ios[(i-1)%ios.length] }
          h2 = h.invert
          rios = pipes.map {|r, w| r }
          wios = pipes.map {|r, w| w }
          child_wfds = wios.map {|w| h2[w].fileno }
          pid = spawn(RUBY, "-e",
                  "[#{child_wfds.join(',')}].each {|fd| IO.new(fd, 'w').puts fd }", h)
          pipes.each {|r, w|
            assert_equal("#{h2[w].fileno}\n", r.gets)
          }
          Process.wait pid;
        }

        with_pipes(5) {|pipes|
          ios = pipes.flatten
          h = {}
          ios.length.times {|i| h[ios[i]] = ios[(i+1)%ios.length] }
          h2 = h.invert
          rios = pipes.map {|r, w| r }
          wios = pipes.map {|r, w| w }
          child_wfds = wios.map {|w| h2[w].fileno }
          pid = spawn(RUBY, "-e",
                  "[#{child_wfds.join(',')}].each {|fd| IO.new(fd, 'w').puts fd }", h)
          pipes.each {|r, w|
            assert_equal("#{h2[w].fileno}\n", r.gets)
          }
          Process.wait pid
        }

        closed_fd = nil
        with_pipes(5) {|pipes|
          io = pipes.last.last
          closed_fd = io.fileno
        }
        assert_raise(Errno::EBADF) { Process.wait spawn(*TRUECOMMAND, closed_fd=>closed_fd) }

        with_pipe {|r, w|
          if w.respond_to?(:"close_on_exec=")
            w.close_on_exec = true
            pid = spawn(RUBY, "-e", "IO.new(#{w.fileno}, 'w').print 'a'", w=>w)
            w.close
            assert_equal("a", r.read)
            Process.wait pid
          end
        }
      end

      system(*ECHO["funya"], :out=>"out")
      assert_equal("funya\n", File.read("out"))
      system(RUBY, '-e', 'STDOUT.reopen(STDERR); puts "henya"', :err=>"out")
      assert_equal("henya\n", File.read("out"))
      IO.popen([*CAT, :in=>"out"]) {|io|
        assert_equal("henya\n", io.read)
      }
    }
  end

  def test_execopts_redirect_dup2_child
    with_tmpchdir {|d|
      Process.wait spawn(RUBY, "-e", "STDERR.print 'err'; STDOUT.print 'out'",
                         STDOUT=>"out", STDERR=>[:child, STDOUT])
      assert_equal("errout", File.read("out"))

      Process.wait spawn(RUBY, "-e", "STDERR.print 'err'; STDOUT.print 'out'",
                         STDERR=>"out", STDOUT=>[:child, STDERR])
      assert_equal("errout", File.read("out"))

      skip "inheritance of fd other than stdin,stdout and stderr is not supported" if windows?
      Process.wait spawn(RUBY, "-e", "STDERR.print 'err'; STDOUT.print 'out'",
                         STDOUT=>"out",
                         STDERR=>[:child, 3],
                         3=>[:child, 4],
                         4=>[:child, STDOUT]
                        )
      assert_equal("errout", File.read("out"))

      IO.popen([RUBY, "-e", "STDERR.print 'err'; STDOUT.print 'out'", STDERR=>[:child, STDOUT]]) {|io|
        assert_equal("errout", io.read)
      }

      assert_raise(ArgumentError) { Process.wait spawn(*TRUECOMMAND, STDOUT=>[:child, STDOUT]) }
      assert_raise(ArgumentError) { Process.wait spawn(*TRUECOMMAND, 3=>[:child, 4], 4=>[:child, 3]) }
      assert_raise(ArgumentError) { Process.wait spawn(*TRUECOMMAND, 3=>[:child, 4], 4=>[:child, 5], 5=>[:child, 3]) }
      assert_raise(ArgumentError) { Process.wait spawn(*TRUECOMMAND, STDOUT=>[:child, 3]) }
    }
  end

  def test_execopts_exec
    with_tmpchdir {|d|
      write_file("s", 'exec "echo aaa", STDOUT=>"foo"')
      pid = spawn RUBY, 's'
      Process.wait pid
      assert_equal("aaa\n", File.read("foo"))
    }
  end

  def test_execopts_popen
    with_tmpchdir {|d|
      IO.popen("#{RUBY} -e 'puts :foo'") {|io| assert_equal("foo\n", io.read) }
      assert_raise(Errno::ENOENT) { IO.popen(["echo bar"]) {} } # assuming "echo bar" command not exist.
      IO.popen(ECHO["baz"]) {|io| assert_equal("baz\n", io.read) }
      assert_raise(ArgumentError) {
        IO.popen([*ECHO["qux"], STDOUT=>STDOUT]) {|io| }
      }
      IO.popen([*ECHO["hoge"], STDERR=>STDOUT]) {|io|
        assert_equal("hoge\n", io.read)
      }
      assert_raise(ArgumentError) {
        IO.popen([*ECHO["fuga"], STDOUT=>"out"]) {|io| }
      }
      skip "inheritance of fd other than stdin,stdout and stderr is not supported" if windows?
      with_pipe {|r, w|
        IO.popen([RUBY, '-e', 'IO.new(3, "w").puts("a"); puts "b"', 3=>w]) {|io|
          assert_equal("b\n", io.read)
        }
        w.close
        assert_equal("a\n", r.read)
      }
      IO.popen([RUBY, '-e', "IO.new(9, 'w').puts(:b)",
               9=>["out2", File::WRONLY|File::CREAT|File::TRUNC]]) {|io|
        assert_equal("", io.read)
      }
      assert_equal("b\n", File.read("out2"))
    }
  end

  def test_popen_fork
    IO.popen("-") {|io|
      if !io
        puts "fooo"
      else
        assert_equal("fooo\n", io.read)
      end
    }
  rescue NotImplementedError
  end

  def test_fd_inheritance
    skip "inheritance of fd other than stdin,stdout and stderr is not supported" if windows?
    with_pipe {|r, w|
      system(RUBY, '-e', 'IO.new(ARGV[0].to_i, "w").puts(:ba)', w.fileno.to_s, w=>w)
      w.close
      assert_equal("ba\n", r.read)
    }
    with_pipe {|r, w|
      Process.wait spawn(RUBY, '-e',
                         'IO.new(ARGV[0].to_i, "w").puts("bi") rescue nil',
                         w.fileno.to_s)
      w.close
      assert_equal("", r.read)
    }
    with_pipe {|r, w|
      with_tmpchdir {|d|
	write_file("s", <<-"End")
	  exec(#{RUBY.dump}, '-e',
	       'IO.new(ARGV[0].to_i, "w").puts("bu") rescue nil',
	       #{w.fileno.to_s.dump}, :close_others=>false)
	End
        w.close_on_exec = false
	Process.wait spawn(RUBY, "s", :close_others=>false)
	w.close
	assert_equal("bu\n", r.read)
      }
    }
    with_pipe {|r, w|
      io = IO.popen([RUBY, "-e", "STDERR.reopen(STDOUT); IO.new(#{w.fileno}, 'w').puts('me')"])
      w.close
      errmsg = io.read
      assert_equal("", r.read)
      assert_not_equal("", errmsg)
      Process.wait
    }
    with_pipe {|r, w|
      errmsg = `#{RUBY} -e "STDERR.reopen(STDOUT); IO.new(#{w.fileno}, 'w').puts(123)"`
      w.close
      assert_equal("", r.read)
      assert_not_equal("", errmsg)
    }
  end

  def test_execopts_close_others
    skip "inheritance of fd other than stdin,stdout and stderr is not supported" if windows?
    with_tmpchdir {|d|
      with_pipe {|r, w|
        system(RUBY, '-e', 'STDERR.reopen("err", "w"); IO.new(ARGV[0].to_i, "w").puts("ma")', w.fileno.to_s, :close_others=>true)
        w.close
        assert_equal("", r.read)
        assert_not_equal("", File.read("err"))
        File.unlink("err")
      }
      with_pipe {|r, w|
        Process.wait spawn(RUBY, '-e', 'STDERR.reopen("err", "w"); IO.new(ARGV[0].to_i, "w").puts("mi")', w.fileno.to_s, :close_others=>true)
        w.close
        assert_equal("", r.read)
        assert_not_equal("", File.read("err"))
        File.unlink("err")
      }
      with_pipe {|r, w|
        w.close_on_exec = false
        Process.wait spawn(RUBY, '-e', 'IO.new(ARGV[0].to_i, "w").puts("bi")', w.fileno.to_s, :close_others=>false)
        w.close
        assert_equal("bi\n", r.read)
      }
      with_pipe {|r, w|
	write_file("s", <<-"End")
	  exec(#{RUBY.dump}, '-e',
	       'STDERR.reopen("err", "w"); IO.new(ARGV[0].to_i, "w").puts("mu")',
	       #{w.fileno.to_s.dump},
	       :close_others=>true)
	End
        Process.wait spawn(RUBY, "s", :close_others=>false)
        w.close
        assert_equal("", r.read)
        assert_not_equal("", File.read("err"))
        File.unlink("err")
      }
      with_pipe {|r, w|
        io = IO.popen([RUBY, "-e", "STDERR.reopen(STDOUT); IO.new(#{w.fileno}, 'w').puts('me')", :close_others=>true])
        w.close
        errmsg = io.read
        assert_equal("", r.read)
        assert_not_equal("", errmsg)
        Process.wait
      }
      with_pipe {|r, w|
        w.close_on_exec = false
        io = IO.popen([RUBY, "-e", "STDERR.reopen(STDOUT); IO.new(#{w.fileno}, 'w').puts('mo')", :close_others=>false])
        w.close
        errmsg = io.read
        assert_equal("mo\n", r.read)
        assert_equal("", errmsg)
        Process.wait
      }
      with_pipe {|r, w|
        w.close_on_exec = false
        io = IO.popen([RUBY, "-e", "STDERR.reopen(STDOUT); IO.new(#{w.fileno}, 'w').puts('mo')", :close_others=>nil])
        w.close
        errmsg = io.read
        assert_equal("mo\n", r.read)
        assert_equal("", errmsg)
        Process.wait
      }

    }
  end

  def test_execopts_redirect_self
    begin
      with_pipe {|r, w|
        w << "haha\n"
        w.close
        r.close_on_exec = true
        IO.popen([RUBY, "-e", "print IO.new(#{r.fileno}, 'r').read", r.fileno=>r.fileno, :close_others=>false]) {|io|
          assert_equal("haha\n", io.read)
        }
      }
    rescue NotImplementedError
      skip "IO#close_on_exec= is not supported"
    end
  end

  def test_execopts_duplex_io
    IO.popen("#{RUBY} -e ''", "r+") {|duplex|
      assert_raise(ArgumentError) { system("#{RUBY} -e ''", duplex=>STDOUT) }
      assert_raise(ArgumentError) { system("#{RUBY} -e ''", STDOUT=>duplex) }
    }
  end

  def test_execopts_modification
    h = {}
    Process.wait spawn(*TRUECOMMAND, h)
    assert_equal({}, h)

    h = {}
    system(*TRUECOMMAND, h)
    assert_equal({}, h)

    h = {}
    io = IO.popen([*TRUECOMMAND, h])
    io.close
    assert_equal({}, h)
  end

  def test_system_noshell
    str = "echo non existing command name which contains spaces"
    assert_nil(system([str, str]))
  end

  def test_spawn_noshell
    str = "echo non existing command name which contains spaces"
    assert_raise(Errno::ENOENT) { spawn([str, str]) }
  end

  def test_popen_noshell
    str = "echo non existing command name which contains spaces"
    assert_raise(Errno::ENOENT) { IO.popen([str, str]) }
  end

  def test_exec_noshell
    with_tmpchdir {|d|
      write_file("s", <<-"End")
	  str = "echo non existing command name which contains spaces"
	  STDERR.reopen(STDOUT)
	  begin
	    exec [str, str]
	  rescue Errno::ENOENT
	    print "Errno::ENOENT success"
	  end
	End
      r = IO.popen([RUBY, "s", :close_others=>false], "r") {|f| f.read}
      assert_equal("Errno::ENOENT success", r)
    }
  end

  def test_system_wordsplit
    with_tmpchdir {|d|
      write_file("script", <<-'End')
        File.open("result", "w") {|t| t << "haha pid=#{$$} ppid=#{Process.ppid}" }
        exit 5
      End
      str = "#{RUBY} script"
      ret = system(str)
      status = $?
      assert_equal(false, ret)
      assert(status.exited?)
      assert_equal(5, status.exitstatus)
      assert_equal("haha pid=#{status.pid} ppid=#{$$}", File.read("result"))
    }
  end

  def test_spawn_wordsplit
    with_tmpchdir {|d|
      write_file("script", <<-'End')
        File.open("result", "w") {|t| t << "hihi pid=#{$$} ppid=#{Process.ppid}" }
        exit 6
      End
      str = "#{RUBY} script"
      pid = spawn(str)
      Process.wait pid
      status = $?
      assert_equal(pid, status.pid)
      assert(status.exited?)
      assert_equal(6, status.exitstatus)
      assert_equal("hihi pid=#{status.pid} ppid=#{$$}", File.read("result"))
    }
  end

  def test_popen_wordsplit
    with_tmpchdir {|d|
      write_file("script", <<-'End')
        print "fufu pid=#{$$} ppid=#{Process.ppid}"
        exit 7
      End
      str = "#{RUBY} script"
      io = IO.popen(str)
      pid = io.pid
      result = io.read
      io.close
      status = $?
      assert_equal(pid, status.pid)
      assert(status.exited?)
      assert_equal(7, status.exitstatus)
      assert_equal("fufu pid=#{status.pid} ppid=#{$$}", result)
    }
  end

  def test_exec_wordsplit
    with_tmpchdir {|d|
      write_file("script", <<-'End')
        File.open("result", "w") {|t|
          if /mswin|bccwin|mingw/ =~ RUBY_PLATFORM
            t << "hehe ppid=#{Process.ppid}"
          else
            t << "hehe pid=#{$$} ppid=#{Process.ppid}"
          end
        }
        exit 6
      End
      write_file("s", <<-"End")
	ruby = #{RUBY.dump}
	exec "\#{ruby} script"
      End
      pid = spawn(RUBY, "s")
      Process.wait pid
      status = $?
      assert_equal(pid, status.pid)
      assert(status.exited?)
      assert_equal(6, status.exitstatus)
      if windows?
        expected = "hehe ppid=#{status.pid}"
      else
        expected = "hehe pid=#{status.pid} ppid=#{$$}"
      end
      assert_equal(expected, File.read("result"))
    }
  end

  def test_system_shell
    with_tmpchdir {|d|
      write_file("script1", <<-'End')
        File.open("result1", "w") {|t| t << "taka pid=#{$$} ppid=#{Process.ppid}" }
        exit 7
      End
      write_file("script2", <<-'End')
        File.open("result2", "w") {|t| t << "taki pid=#{$$} ppid=#{Process.ppid}" }
        exit 8
      End
      ret = system("#{RUBY} script1 || #{RUBY} script2")
      status = $?
      assert_equal(false, ret)
      assert(status.exited?)
      result1 = File.read("result1")
      result2 = File.read("result2")
      assert_match(/\Ataka pid=\d+ ppid=\d+\z/, result1)
      assert_match(/\Ataki pid=\d+ ppid=\d+\z/, result2)
      assert_not_equal(result1[/\d+/].to_i, status.pid)

      if windows?
        Dir.mkdir(path = "path with space")
        write_file(bat = path + "/bat test.bat", "@echo %1>out")
        system(bat, "foo 'bar'")
        assert_equal(%["foo 'bar'"\n], File.read("out"), '[ruby-core:22960]')
        system(%[#{bat.dump} "foo 'bar'"])
        assert_equal(%["foo 'bar'"\n], File.read("out"), '[ruby-core:22960]')
      end
    }
  end

  def test_spawn_shell
    with_tmpchdir {|d|
      write_file("script1", <<-'End')
        File.open("result1", "w") {|t| t << "taku pid=#{$$} ppid=#{Process.ppid}" }
        exit 7
      End
      write_file("script2", <<-'End')
        File.open("result2", "w") {|t| t << "take pid=#{$$} ppid=#{Process.ppid}" }
        exit 8
      End
      pid = spawn("#{RUBY} script1 || #{RUBY} script2")
      Process.wait pid
      status = $?
      assert(status.exited?)
      assert(!status.success?)
      result1 = File.read("result1")
      result2 = File.read("result2")
      assert_match(/\Ataku pid=\d+ ppid=\d+\z/, result1)
      assert_match(/\Atake pid=\d+ ppid=\d+\z/, result2)
      assert_not_equal(result1[/\d+/].to_i, status.pid)

      if windows?
        Dir.mkdir(path = "path with space")
        write_file(bat = path + "/bat test.bat", "@echo %1>out")
        pid = spawn(bat, "foo 'bar'")
        Process.wait pid
        status = $?
        assert(status.exited?)
        assert(status.success?)
        assert_equal(%["foo 'bar'"\n], File.read("out"), '[ruby-core:22960]')
        pid = spawn(%[#{bat.dump} "foo 'bar'"])
        Process.wait pid
        status = $?
        assert(status.exited?)
        assert(status.success?)
        assert_equal(%["foo 'bar'"\n], File.read("out"), '[ruby-core:22960]')
      end
    }
  end

  def test_popen_shell
    with_tmpchdir {|d|
      write_file("script1", <<-'End')
        puts "tako pid=#{$$} ppid=#{Process.ppid}"
        exit 7
      End
      write_file("script2", <<-'End')
        puts "tika pid=#{$$} ppid=#{Process.ppid}"
        exit 8
      End
      io = IO.popen("#{RUBY} script1 || #{RUBY} script2")
      result = io.read
      io.close
      status = $?
      assert(status.exited?)
      assert(!status.success?)
      assert_match(/\Atako pid=\d+ ppid=\d+\ntika pid=\d+ ppid=\d+\n\z/, result)
      assert_not_equal(result[/\d+/].to_i, status.pid)

      if windows?
        Dir.mkdir(path = "path with space")
        write_file(bat = path + "/bat test.bat", "@echo %1")
        r = IO.popen([bat, "foo 'bar'"]) {|f| f.read}
        assert_equal(%["foo 'bar'"\n], r, '[ruby-core:22960]')
        r = IO.popen(%[#{bat.dump} "foo 'bar'"]) {|f| f.read}
        assert_equal(%["foo 'bar'"\n], r, '[ruby-core:22960]')
      end
    }
  end

  def test_exec_shell
    with_tmpchdir {|d|
      write_file("script1", <<-'End')
        File.open("result1", "w") {|t| t << "tiki pid=#{$$} ppid=#{Process.ppid}" }
        exit 7
      End
      write_file("script2", <<-'End')
        File.open("result2", "w") {|t| t << "tiku pid=#{$$} ppid=#{Process.ppid}" }
        exit 8
      End
      write_file("s", <<-"End")
	ruby = #{RUBY.dump}
	exec("\#{ruby} script1 || \#{ruby} script2")
      End
      pid = spawn RUBY, "s"
      Process.wait pid
      status = $?
      assert(status.exited?)
      assert(!status.success?)
      result1 = File.read("result1")
      result2 = File.read("result2")
      assert_match(/\Atiki pid=\d+ ppid=\d+\z/, result1)
      assert_match(/\Atiku pid=\d+ ppid=\d+\z/, result2)
      assert_not_equal(result1[/\d+/].to_i, status.pid)
    }
  end

  def test_argv0
    with_tmpchdir {|d|
      assert_equal(false, system([RUBY, "asdfg"], "-e", "exit false"))
      assert_equal(true, system([RUBY, "zxcvb"], "-e", "exit true"))

      Process.wait spawn([RUBY, "poiu"], "-e", "exit 4")
      assert_equal(4, $?.exitstatus)

      assert_equal("1", IO.popen([[RUBY, "qwerty"], "-e", "print 1"]).read)
      Process.wait

      write_file("s", <<-"End")
        exec([#{RUBY.dump}, "lkjh"], "-e", "exit 5")
      End
      pid = spawn RUBY, "s"
      Process.wait pid
      assert_equal(5, $?.exitstatus)
    }
  end

  def with_stdin(filename)
    open(filename) {|f|
      begin
        old = STDIN.dup
        begin
          STDIN.reopen(filename)
          yield
        ensure
          STDIN.reopen(old)
        end
      ensure
        old.close
      end
    }
  end

  def test_argv0_noarg
    with_tmpchdir {|d|
      open("t", "w") {|f| f.print "exit true" }
      open("f", "w") {|f| f.print "exit false" }

      with_stdin("t") { assert_equal(true, system([RUBY, "qaz"])) }
      with_stdin("f") { assert_equal(false, system([RUBY, "wsx"])) }

      with_stdin("t") { Process.wait spawn([RUBY, "edc"]) }
      assert($?.success?)
      with_stdin("f") { Process.wait spawn([RUBY, "rfv"]) }
      assert(!$?.success?)

      with_stdin("t") { IO.popen([[RUBY, "tgb"]]) {|io| assert_equal("", io.read) } }
      assert($?.success?)
      with_stdin("f") { IO.popen([[RUBY, "yhn"]]) {|io| assert_equal("", io.read) } }
      assert(!$?.success?)

      status = run_in_child "STDIN.reopen('t'); exec([#{RUBY.dump}, 'ujm'])"
      assert(status.success?)
      status = run_in_child "STDIN.reopen('f'); exec([#{RUBY.dump}, 'ik,'])"
      assert(!status.success?)
    }
  end

  def test_status
    with_tmpchdir do
      s = run_in_child("exit 1")
      assert_equal("#<Process::Status: pid #{ s.pid } exit #{ s.exitstatus }>", s.inspect)

      assert_equal(s, s)
      assert_equal(s, s.to_i)

      assert_equal(s.to_i & 0x55555555, s & 0x55555555)
      assert_equal(s.to_i >> 1, s >> 1)
      assert_equal(false, s.stopped?)
      assert_equal(nil, s.stopsig)
    end
  end

  def test_status_kill
    return unless Process.respond_to?(:kill)
    return unless Signal.list.include?("QUIT")

    with_tmpchdir do
      write_file("foo", "sleep 30")
      pid = spawn(RUBY, "foo")
      Thread.new { sleep 1; Process.kill(:SIGQUIT, pid) }
      Process.wait(pid)
      s = $?
      assert_equal([false, true, false],
                   [s.exited?, s.signaled?, s.stopped?],
                   "[s.exited?, s.signaled?, s.stopped?]")
      assert_send(
        [["#<Process::Status: pid #{ s.pid } SIGQUIT (signal #{ s.termsig })>",
          "#<Process::Status: pid #{ s.pid } SIGQUIT (signal #{ s.termsig }) (core dumped)>"],
         :include?,
         s.inspect])
      assert_equal(false, s.exited?)
      assert_equal(nil, s.success?)
    end
  end

  def test_wait_without_arg
    with_tmpchdir do
      write_file("foo", "sleep 0.1")
      pid = spawn(RUBY, "foo")
      assert_equal(pid, Process.wait)
    end
  end

  def test_wait2
    with_tmpchdir do
      write_file("foo", "sleep 0.1")
      pid = spawn(RUBY, "foo")
      assert_equal([pid, 0], Process.wait2)
    end
  end

  def test_waitall
    with_tmpchdir do
      write_file("foo", "sleep 0.1")
      ps = (0...3).map { spawn(RUBY, "foo") }.sort
      ss = Process.waitall.sort
      ps.zip(ss) do |p1, (p2, s)|
        assert_equal(p1, p2)
        assert_equal(p1, s.pid)
      end
    end
  end

  def test_abort
    with_tmpchdir do
      s = run_in_child("abort")
      assert_not_equal(0, s.exitstatus)
    end
  end

  def test_sleep
    assert_raise(ArgumentError) { sleep(1, 1) }
  end

  def test_getpgid
    assert_kind_of(Integer, Process.getpgid(Process.ppid))
  rescue NotImplementedError
  end

  def test_getpriority
    assert_kind_of(Integer, Process.getpriority(Process::PRIO_PROCESS, $$))
  rescue NameError, NotImplementedError
  end

  def test_setpriority
    if defined? Process::PRIO_USER
      assert_nothing_raised do
        pr = Process.getpriority(Process::PRIO_PROCESS, $$)
        Process.setpriority(Process::PRIO_PROCESS, $$, pr)
      end
    end
  end

  def test_getuid
    assert_kind_of(Integer, Process.uid)
  end

  def test_groups
    gs = Process.groups
    assert_instance_of(Array, gs)
    gs.each {|g| assert_kind_of(Integer, g) }
  rescue NotImplementedError
  end

  def test_maxgroups
    assert_kind_of(Integer, Process.maxgroups)
  rescue NotImplementedError
  end

  def test_geteuid
    assert_kind_of(Integer, Process.euid)
  end

  def test_seteuid
    assert_nothing_raised(TypeError) {Process.euid += 0}
  rescue NotImplementedError
  end

  def test_getegid
    assert_kind_of(Integer, Process.egid)
  end

  def test_setegid
    assert_nothing_raised(TypeError) {Process.egid += 0}
  rescue NotImplementedError
  end

  def test_uid_re_exchangeable_p
    r = Process::UID.re_exchangeable?
    assert(true == r || false == r)
  end

  def test_gid_re_exchangeable_p
    r = Process::GID.re_exchangeable?
    assert(true == r || false == r)
  end

  def test_uid_sid_available?
    r = Process::UID.sid_available?
    assert(true == r || false == r)
  end

  def test_gid_sid_available?
    r = Process::GID.sid_available?
    assert(true == r || false == r)
  end

  def test_pst_inspect
    assert_nothing_raised { Process::Status.allocate.inspect }
  end

  def test_wait_and_sigchild
    if /freebsd|openbsd/ =~ RUBY_PLATFORM
      # this relates #4173
      # When ruby can use 2 cores, signal and wait4 may miss the signal.
      skip "this fails on FreeBSD and OpenBSD on multithreaded environment"
    end
    signal_received = []
    Signal.trap(:CHLD)  { signal_received << true }
    pid = fork { sleep 0.1; exit }
    Thread.start { raise }
    Process.wait pid
    sleep 0.1
    assert_equal [true], signal_received, " [ruby-core:19744]"
  rescue NotImplementedError, ArgumentError
  ensure
    begin
      Signal.trap(:CHLD, 'DEFAULT')
    rescue ArgumentError
    end
  end

  def test_no_curdir
    with_tmpchdir {|d|
      Dir.mkdir("vd")
      status = nil
      Dir.chdir("vd") {
        dir = "#{d}/vd"
        # OpenSolaris cannot remove the current directory.
        system(RUBY, "--disable-gems", "-e", "Dir.chdir '..'; Dir.rmdir #{dir.dump}")
        system({"RUBYLIB"=>nil}, RUBY, "--disable-gems", "-e", "exit true")
        status = $?
      }
      assert(status.success?, "[ruby-dev:38105]")
    }
  end unless /mswin|bccwin|mingw/ =~ RUBY_PLATFORM

  def test_fallback_to_sh
    feature = '[ruby-core:32745]'
    with_tmpchdir do |d|
      open("tmp_script.#{$$}", "w") {|f| f.puts ": ;"; f.chmod(0755)}
      assert_not_nil(pid = Process.spawn("./tmp_script.#{$$}"), feature)
      wpid, st = Process.waitpid2(pid)
      assert_equal([pid, true], [wpid, st.success?], feature)

      open("tmp_script.#{$$}", "w") {|f| f.puts "echo $#: $@"; f.chmod(0755)}
      result = IO.popen(["./tmp_script.#{$$}", "a b", "c"]) {|f| f.read}
      assert_equal("2: a b c\n", result, feature)
    end
  end if File.executable?("/bin/sh")

  def test_too_long_path
    bug4314 = '[ruby-core:34842]'
    exs = [Errno::ENOENT]
    exs << Errno::E2BIG if defined?(Errno::E2BIG)
    assert_raise(*exs, bug4314) {Process.spawn("a" * 10_000_000)}
  end

  def test_too_long_path2
    bug4315 = '[ruby-core:34833]'
    exs = [Errno::ENOENT]
    exs << Errno::E2BIG if defined?(Errno::E2BIG)
    assert_raise(*exs, bug4315) {Process.spawn('"a"|'*10_000_000)}
  end

  def test_system_sigpipe
    return if windows?

    pid = 0

    with_tmpchdir do
      assert_nothing_raised('[ruby-dev:12261]') do
        timeout(3) do
          pid = spawn('yes | ls')
          Process.waitpid pid
        end
      end
    end
  ensure
    Process.kill(:KILL, pid) if (pid != 0) rescue false
  end

  if Process.respond_to?(:daemon)
    def test_daemon_default
      data = IO.popen("-", "r+") do |f|
        break f.read if f
        Process.daemon
        puts "ng"
      end
      assert_equal("", data)
    end

    def test_daemon_noclose
      data = IO.popen("-", "r+") do |f|
        break f.read if f
        Process.daemon(false, true)
        puts "ok", Dir.pwd
      end
      assert_equal("ok\n/\n", data)
    end

    def test_daemon_nochdir_noclose
      data = IO.popen("-", "r+") do |f|
        break f.read if f
        Process.daemon(true, true)
        puts "ok", Dir.pwd
      end
      assert_equal("ok\n#{Dir.pwd}\n", data)
    end

    def test_daemon_readwrite
      data = IO.popen("-", "r+") do |f|
        if f
          f.puts "ok?"
          break f.read
        end
        Process.daemon(true, true)
        puts STDIN.gets
      end
      assert_equal("ok?\n", data)
    end

    if File.directory?("/proc/self/task")
      def test_daemon_no_threads
        pid, data = IO.popen("-", "r+") do |f|
          break f.pid, f.readlines if f
          Process.daemon(true, true)
          puts Dir.entries("/proc/self/task") - %W[. ..]
        end
        bug4920 = '[ruby-dev:43873]'
        assert_equal(2, data.size, bug4920)
        assert_not_include(data.map(&:to_i), pid)
      end
    end
  end

  def test_popen_cloexec
    return unless defined? Fcntl::FD_CLOEXEC
    IO.popen([RUBY, "-e", ""]) {|io|
      assert(io.close_on_exec?)
    }
  end
end
