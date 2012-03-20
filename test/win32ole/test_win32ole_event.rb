begin
  require 'win32ole'
rescue LoadError
end
require 'test/unit'

def ado_installed?
  installed = false
  if defined?(WIN32OLE)
    db = nil
    begin
      db = WIN32OLE.new('ADODB.Connection')
      db.connectionString = "Driver={Microsoft Text Driver (*.txt; *.csv)};DefaultDir=.;"
      db.open
      db.close
      db = nil
      installed = true
    rescue
    end
  end
  installed
end

if defined?(WIN32OLE_EVENT)
  dotest = ado_installed?
  if !dotest
    STDERR.puts("\n#{__FILE__} skipped(ActiveX Data Object Library not found.)")
  end
  if dotest
    class TestWIN32OLE_EVENT < Test::Unit::TestCase
      CONNSTR="Driver={Microsoft Text Driver (*.txt; *.csv)};DefaultDir=.;"
      module ADO
      end
      def message_loop
        WIN32OLE_EVENT.message_loop
      end

      def default_handler(event, *args)
        @event += event
      end

      def setup
        @db = WIN32OLE.new('ADODB.Connection')
        if !defined?(ADO::AdStateOpen)
          WIN32OLE.const_load(@db, ADO)
        end
        @db.connectionString = CONNSTR
        @event = ""
        @event2 = ""
        @event3 = ""
      end

      def test_s_new
        assert_raise(TypeError) {
          ev = WIN32OLE_EVENT.new("A")
        }
      end

      def test_s_new_without_itf
        ev = WIN32OLE_EVENT.new(@db)
        ev.on_event {|*args| default_handler(*args)}
        @db.open
        @db.close
        10.times do |i|
          WIN32OLE_EVENT.new(@db)
          GC.start
          message_loop
          @db.open
          message_loop
          @db.close
        end
        assert_match(/WillConnect/, @event)
      end

      def test_on_event
        ev = WIN32OLE_EVENT.new(@db, 'ConnectionEvents')
        ev.on_event {|*args| default_handler(*args)}
        @db.open
        message_loop
        assert_match(/WillConnect/, @event)
      end

      def test_on_event_symbol
        ev = WIN32OLE_EVENT.new(@db)
        ev.on_event(:WillConnect) {|*args|
          handler1
        }
        @db.open
        message_loop
        assert_equal("handler1", @event2)
      end

      def test_on_event2
        ev = WIN32OLE_EVENT.new(@db, 'ConnectionEvents')
        ev.on_event('WillConnect') {|*args| handler1}
        ev.on_event('WillConnect') {|*args| handler2}
        @db.open
        message_loop
        assert_equal("handler2", @event2)
      end

      def test_on_event3
        ev = WIN32OLE_EVENT.new(@db, 'ConnectionEvents')
        ev.on_event('WillConnect') {|*args| handler1}
        ev.on_event('WillConnect') {|*args| handler2}
        @db.open
        message_loop
        assert_equal("handler2", @event2)
      end

      def test_on_event4
        ev = WIN32OLE_EVENT.new(@db, 'ConnectionEvents')
        ev.on_event{|*args| handler1}
        ev.on_event{|*args| handler2}
        ev.on_event('WillConnect'){|*args| handler3(*args)}
        @db.open
        message_loop
        assert_equal(CONNSTR, @event3)
        assert("handler2", @event2)
      end

      def test_on_event5
        ev = WIN32OLE_EVENT.new(@db, 'ConnectionEvents')
        ev.on_event {|*args| default_handler(*args)}
        ev.on_event('WillConnect'){|*args| handler3(*args)}
        @db.open
        message_loop
        assert_match(/ConnectComplete/, @event)
        assert(/WillConnect/ !~ @event)
        assert_equal(CONNSTR, @event3)
      end

      def test_unadvise
        ev = WIN32OLE_EVENT.new(@db, 'ConnectionEvents')
        ev.on_event {|*args| default_handler(*args)}
        @db.open
        message_loop
        assert_match(/WillConnect/, @event)
        ev.unadvise
        @event = ""
        @db.close
        @db.open
        message_loop
        assert_equal("", @event);
        assert_raise(WIN32OLERuntimeError) {
          ev.on_event {|*args| default_handler(*args)}
        }
      end

      def test_non_exist_event
        assert_raise(RuntimeError) {
          ev = WIN32OLE_EVENT.new(@db, 'XXXX')
        }
        dict = WIN32OLE.new('Scripting.Dictionary')
        assert_raise(RuntimeError) {
          ev = WIN32OLE_EVENT.new(dict)
        }
      end

      def test_on_event_with_outargs
        ev = WIN32OLE_EVENT.new(@db)
        @db.connectionString = 'XXX' # set illegal connection string
        assert_raise(WIN32OLERuntimeError) {
          @db.open
        }
        ev.on_event_with_outargs('WillConnect'){|*args|
          args.last[0] = CONNSTR # ConnectionString = CONNSTR
        }
        @db.open
        message_loop
        assert(true)
      end

      def test_on_event_hash_return
        ev = WIN32OLE_EVENT.new(@db)
        ev.on_event('WillConnect'){|*args|
          {:return => 1, :ConnectionString => CONNSTR}
        }
        @db.connectionString = 'XXX'
        @db.open
        assert(true)
      end

      def test_on_event_hash_return2
        ev = WIN32OLE_EVENT.new(@db)
        ev.on_event('WillConnect'){|*args|
          {:ConnectionString => CONNSTR}
        }
        @db.connectionString = 'XXX'
        @db.open
        assert(true)
      end

      def test_on_event_hash_return3
        ev = WIN32OLE_EVENT.new(@db)
        ev.on_event('WillConnect'){|*args|
          {'ConnectionString' => CONNSTR}
        }
        @db.connectionString = 'XXX'
        @db.open
        assert(true)
      end

      def test_on_event_hash_return4
        ev = WIN32OLE_EVENT.new(@db)
        ev.on_event('WillConnect'){|*args|
          {'return' => 1, 'ConnectionString' => CONNSTR}
        }
        @db.connectionString = 'XXX'
        @db.open
        assert(true)
      end

      def test_on_event_hash_return5
        ev = WIN32OLE_EVENT.new(@db)
        ev.on_event('WillConnect'){|*args|
          {0 => CONNSTR}
        }
        @db.connectionString = 'XXX'
        @db.open
        assert(true)
      end

      def test_off_event
        ev = WIN32OLE_EVENT.new(@db)
        ev.on_event{handler1}
        ev.off_event
        @db.open
        message_loop
        assert_equal("", @event2)
      end

      def test_off_event_arg
        ev = WIN32OLE_EVENT.new(@db)
        ev.on_event('WillConnect'){handler1}
        ev.off_event('WillConnect')
        @db.open
        message_loop
        assert_equal("", @event2)
      end

      def test_off_event_arg2
        ev = WIN32OLE_EVENT.new(@db)
        ev.on_event('WillConnect'){handler1}
        ev.on_event('ConnectComplete'){handler1}
        ev.off_event('WillConnect')
        @db.open
        message_loop
        assert_equal("handler1", @event2)
      end

      def test_off_event_sym_arg
        ev = WIN32OLE_EVENT.new(@db)
        ev.on_event('WillConnect'){handler1}
        ev.off_event(:WillConnect)
        @db.open
        message_loop
        assert_equal("", @event2)
      end

      def handler1
        @event2 = "handler1"
      end

      def handler2
        @event2 = "handler2"
      end

      def handler3(*arg)
        @event3 += arg[0]
      end

      def teardown
        if @db && @db.state == ADO::AdStateOpen
          @db.close
        end
        message_loop
        @db = nil
      end

      class Handler1
        attr_reader :val1, :val2, :val3, :val4
        def initialize
          @val1 = nil
          @val2 = nil
          @val3 = nil
          @val4 = nil
        end
        def onWillConnect(conn, uid, pwd, opts, stat, pconn)
          @val1 = conn
        end
        def onConnectComplete(err, stat, pconn)
          @val2 = err
          @val3 = stat
        end
        def onInfoMessage(err, stat, pconn)
          @val4 = stat
        end
      end

      class Handler2
        attr_reader :ev
        def initialize
          @ev = ""
        end
        def method_missing(ev, *arg)
          @ev += ev
        end
      end

      def test_handler1
        ev = WIN32OLE_EVENT.new(@db)
        h1 = Handler1.new
        ev.handler = h1
        @db.open
        message_loop
        assert_equal(CONNSTR, h1.val1)
        assert_equal(h1.val1, ev.handler.val1)
        assert_equal(nil, h1.val2)
        assert_equal(ADO::AdStateOpen, h1.val3)
        assert_equal(ADO::AdStateOpen, h1.val4)
      end

      def test_handler2
        ev = WIN32OLE_EVENT.new(@db)
        h2 = Handler2.new
        ev.handler = h2
        @db.open
        message_loop
        assert(h2.ev != "")
      end
    end
  end
end
