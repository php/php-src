assert_equal '0', %q{
  begin
    GC.stress = true
    pid = fork {}
    Process.wait pid
    $?.to_i
  rescue NotImplementedError
    0
  end
}, '[ruby-dev:32404]'

assert_finish 10, %q{
  begin
    children = (1..10).map{
      Thread.start{fork{}}.value
    }
    while !children.empty? and pid = Process.wait
      children.delete(pid)
    end
  rescue NotImplementedError
  end
}, '[ruby-core:22158]'

assert_normal_exit(<<'End', '[ruby-dev:37934]')
  Thread.new { sleep 1; Thread.kill Thread.main }
  Process.setrlimit(:NPROC, 1)
  fork {}
End

assert_equal 'ok', %q{
  begin
    if pid1 = fork
      sleep 1
      Process.kill("USR1", pid1)
      _, s = Process.wait2(pid1)
      s.success? ? :ok : :ng
    else
      if pid2 = fork
        trap("USR1") { Time.now.to_s }
        Process.wait2(pid2)
      else
        sleep 2
      end
      exit 0
    end
  rescue NotImplementedError
    :ok
  end
}, '[ruby-core:28924]'

assert_equal '[1, 2]', %q{
  a = []
  trap(:INT) { a.push(1) }
  trap(:TERM) { a.push(2) }
  pid = $$
  begin
    fork do
      sleep 0.5
      Process.kill(:INT, pid)
      Process.kill(:TERM, pid)
    end

    sleep 1
    a.sort
  rescue NotImplementedError
    [1, 2]
  end
}, '[ruby-dev:44005] [Ruby 1.9 - Bug #4950]'

