require 'net/pop'
require 'test/unit'
require 'digest/md5'

class TestPOP < Test::Unit::TestCase
  def setup
    @users = {'user' => 'pass' }
    @ok_user = 'user'
    @stamp_base = "#{$$}.#{Time.now.to_i}@localhost"
  end

  def test_pop_auth_ok
    pop_test(false) do |pop|
      assert_instance_of Net::POP3, pop
      assert_nothing_raised do
        pop.start(@ok_user, @users[@ok_user])
      end
    end
  end

  def test_pop_auth_ng
    pop_test(false) do |pop|
      assert_instance_of Net::POP3, pop
      assert_raise Net::POPAuthenticationError do
        pop.start(@ok_user, 'bad password')
      end
    end
  end

  def test_apop_ok
    pop_test(@stamp_base) do |pop|
      assert_instance_of Net::APOP, pop
      assert_nothing_raised do
        pop.start(@ok_user, @users[@ok_user])
      end
    end
  end

  def test_apop_ng
    pop_test(@stamp_base) do |pop|
      assert_instance_of Net::APOP, pop
      assert_raise Net::POPAuthenticationError do
        pop.start(@ok_user, 'bad password')
      end
    end
  end

  def test_apop_invalid
    pop_test("\x80"+@stamp_base) do |pop|
      assert_instance_of Net::APOP, pop
      assert_raise Net::POPAuthenticationError do
        pop.start(@ok_user, @users[@ok_user])
      end
    end
  end

  def test_apop_invalid_at
    pop_test(@stamp_base.sub('@', '.')) do |pop|
      assert_instance_of Net::APOP, pop
      e = assert_raise Net::POPAuthenticationError do
        pop.start(@ok_user, @users[@ok_user])
      end
    end
  end

  def pop_test(apop=false)
    host = 'localhost'
    server = TCPServer.new(host, 0)
    port = server.addr[1]
    thread = Thread.start do
      sock = server.accept
      begin
        pop_server_loop(sock, apop)
      ensure
        sock.close
      end
    end
    begin
      pop = Net::POP3::APOP(apop).new(host, port)
      #pop.set_debug_output $stderr
      yield pop
    ensure
      begin
        pop.finish
      rescue IOError
        raise unless $!.message == "POP session not yet started"
      end
    end
  ensure
    server.close
    thread.value
  end

  def pop_server_loop(sock, apop)
    if apop
      sock.print "+OK ready <#{apop}>\r\n"
    else
      sock.print "+OK ready\r\n"
    end
    user = nil
    while line = sock.gets
      case line
      when /^USER (.+)\r\n/
        user = $1
        if @users.key?(user)
          sock.print "+OK\r\n"
        else
          sock.print "-ERR unknown user\r\n"
        end
      when /^PASS (.+)\r\n/
        if @users[user] == $1
          sock.print "+OK\r\n"
        else
          sock.print "-ERR invalid password\r\n"
        end
      when /^APOP (.+) (.+)\r\n/
        user = $1
        if apop && Digest::MD5.hexdigest("<#{apop}>#{@users[user]}") == $2
          sock.print "+OK\r\n"
        else
          sock.print "-ERR authentication failed\r\n"
        end
      when /^QUIT/
        sock.print "+OK bye\r\n"
        return
      else
        sock.print "-ERR command not recognized\r\n"
        return
      end
    end
  end
end
