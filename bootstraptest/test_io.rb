assert_finish 5, %q{
  r, w = IO.pipe
  t1 = Thread.new { r.sysread(1) }
  t2 = Thread.new { r.sysread(1) }
  sleep 0.1
  w.write "a"
  sleep 0.1
  w.write "a"
}, '[ruby-dev:31866]'

assert_finish 10, %q{
  begin
    require "io/nonblock"
    require "timeout"
    timeout(3) do
      r, w = IO.pipe
      w.nonblock?
      w.nonblock = true
      w.write_nonblock("a" * 100000)
      w.nonblock = false
      t1 = Thread.new { w.write("b" * 4096) }
      t2 = Thread.new { w.write("c" * 4096) }
      sleep 0.5
      r.sysread(4096).length
      sleep 0.5
      r.sysread(4096).length
      t1.join
      t2.join
    end
  rescue LoadError, TimeoutError, NotImplementedError
  end
}, '[ruby-dev:32566]'

assert_finish 1, %q{
  r, w = IO.pipe
  Thread.new {
  w << "ab"
  sleep 0.1
  w << "ab"
  }
  r.gets("abab")
}

assert_equal 'ok', %q{
  require 'tmpdir'
  begin
    tmpname = "#{Dir.tmpdir}/ruby-btest-#{$$}-#{rand(0x100000000).to_s(36)}"
    rw = File.open(tmpname, File::RDWR|File::CREAT|File::EXCL)
  rescue Errno::EEXIST
    retry
  end
  save = STDIN.dup
  STDIN.reopen(rw)
  STDIN.reopen(save)
  rw.close
  File.unlink(tmpname)
  :ok
}

assert_equal 'ok', %q{
  require 'tmpdir'
  begin
    tmpname = "#{Dir.tmpdir}/ruby-btest-#{$$}-#{rand(0x100000000).to_s(36)}"
    rw = File.open(tmpname, File::RDWR|File::CREAT|File::EXCL)
  rescue Errno::EEXIST
    retry
  end
  save = STDIN.dup
  STDIN.reopen(rw)
  STDIN.print "a"
  STDIN.reopen(save)
  rw.close
  File.unlink(tmpname)
  :ok
}

assert_normal_exit %q{
  ARGF.set_encoding "foo"
}

10.times do
  assert_normal_exit %q{
    at_exit { p :foo }

    megacontent = "abc" * 12345678
    #File.open("megasrc", "w") {|f| f << megacontent }

    Thread.new { sleep rand*0.2; Process.kill(:INT, $$) }

    r1, w1 = IO.pipe
    r2, w2 = IO.pipe
    t1 = Thread.new { w1 << megacontent; w1.close }
    t2 = Thread.new { r2.read; r2.close }
    IO.copy_stream(r1, w2) rescue nil
    w2.close
    r1.close
    t1.join
    t2.join
  }, 'megacontent-copy_stream', ["INT"], :timeout => 10 or break
end

assert_normal_exit %q{
  r, w = IO.pipe
  STDOUT.reopen(w)
  STDOUT.reopen(__FILE__, "r")
}, '[ruby-dev:38131]'
