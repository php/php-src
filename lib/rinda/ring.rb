#
# Note: Rinda::Ring API is unstable.
#
require 'drb/drb'
require 'rinda/rinda'
require 'thread'

module Rinda

  ##
  # The default port Ring discovery will use.

  Ring_PORT = 7647

  ##
  # A RingServer allows a Rinda::TupleSpace to be located via UDP broadcasts.
  # Service location uses the following steps:
  #
  # 1. A RingServer begins listening on the broadcast UDP address.
  # 2. A RingFinger sends a UDP packet containing the DRb URI where it will
  #    listen for a reply.
  # 3. The RingServer receives the UDP packet and connects back to the
  #    provided DRb URI with the DRb service.

  class RingServer

    include DRbUndumped

    ##
    # Advertises +ts+ on the UDP broadcast address at +port+.

    def initialize(ts, port=Ring_PORT)
      @ts = ts
      @soc = UDPSocket.open
      @soc.bind('', port)
      @w_service = write_service
      @r_service = reply_service
    end

    ##
    # Creates a thread that picks up UDP packets and passes them to do_write
    # for decoding.

    def write_service
      Thread.new do
        loop do
          msg = @soc.recv(1024)
          do_write(msg)
        end
      end
    end

    ##
    # Extracts the response URI from +msg+ and adds it to TupleSpace where it
    # will be picked up by +reply_service+ for notification.

    def do_write(msg)
      Thread.new do
        begin
          tuple, sec = Marshal.load(msg)
          @ts.write(tuple, sec)
        rescue
        end
      end
    end

    ##
    # Creates a thread that notifies waiting clients from the TupleSpace.

    def reply_service
      Thread.new do
        loop do
          do_reply
        end
      end
    end

    ##
    # Pulls lookup tuples out of the TupleSpace and sends their DRb object the
    # address of the local TupleSpace.

    def do_reply
      tuple = @ts.take([:lookup_ring, DRbObject])
      Thread.new { tuple[1].call(@ts) rescue nil}
    rescue
    end

  end

  ##
  # RingFinger is used by RingServer clients to discover the RingServer's
  # TupleSpace.  Typically, all a client needs to do is call
  # RingFinger.primary to retrieve the remote TupleSpace, which it can then
  # begin using.

  class RingFinger

    @@broadcast_list = ['<broadcast>', 'localhost']

    @@finger = nil

    ##
    # Creates a singleton RingFinger and looks for a RingServer.  Returns the
    # created RingFinger.

    def self.finger
      unless @@finger
        @@finger = self.new
        @@finger.lookup_ring_any
      end
      @@finger
    end

    ##
    # Returns the first advertised TupleSpace.

    def self.primary
      finger.primary
    end

    ##
    # Contains all discovered TupleSpaces except for the primary.

    def self.to_a
      finger.to_a
    end

    ##
    # The list of addresses where RingFinger will send query packets.

    attr_accessor :broadcast_list

    ##
    # The port that RingFinger will send query packets to.

    attr_accessor :port

    ##
    # Contain the first advertised TupleSpace after lookup_ring_any is called.

    attr_accessor :primary

    ##
    # Creates a new RingFinger that will look for RingServers at +port+ on
    # the addresses in +broadcast_list+.

    def initialize(broadcast_list=@@broadcast_list, port=Ring_PORT)
      @broadcast_list = broadcast_list || ['localhost']
      @port = port
      @primary = nil
      @rings = []
    end

    ##
    # Contains all discovered TupleSpaces except for the primary.

    def to_a
      @rings
    end

    ##
    # Iterates over all discovered TupleSpaces starting with the primary.

    def each
      lookup_ring_any unless @primary
      return unless @primary
      yield(@primary)
      @rings.each { |x| yield(x) }
    end

    ##
    # Looks up RingServers waiting +timeout+ seconds.  RingServers will be
    # given +block+ as a callback, which will be called with the remote
    # TupleSpace.

    def lookup_ring(timeout=5, &block)
      return lookup_ring_any(timeout) unless block_given?

      msg = Marshal.dump([[:lookup_ring, DRbObject.new(block)], timeout])
      @broadcast_list.each do |it|
        soc = UDPSocket.open
        begin
          soc.setsockopt(Socket::SOL_SOCKET, Socket::SO_BROADCAST, true)
          soc.send(msg, 0, it, @port)
        rescue
          nil
        ensure
          soc.close
        end
      end
      sleep(timeout)
    end

    ##
    # Returns the first found remote TupleSpace.  Any further recovered
    # TupleSpaces can be found by calling +to_a+.

    def lookup_ring_any(timeout=5)
      queue = Queue.new

      Thread.new do
        self.lookup_ring(timeout) do |ts|
          queue.push(ts)
        end
        queue.push(nil)
        while it = queue.pop
          @rings.push(it)
        end
      end

      @primary = queue.pop
      raise('RingNotFound') if @primary.nil?
      @primary
    end

  end

  ##
  # RingProvider uses a RingServer advertised TupleSpace as a name service.
  # TupleSpace clients can register themselves with the remote TupleSpace and
  # look up other provided services via the remote TupleSpace.
  #
  # Services are registered with a tuple of the format [:name, klass,
  # DRbObject, description].

  class RingProvider

    ##
    # Creates a RingProvider that will provide a +klass+ service running on
    # +front+, with a +description+.  +renewer+ is optional.

    def initialize(klass, front, desc, renewer = nil)
      @tuple = [:name, klass, front, desc]
      @renewer = renewer || Rinda::SimpleRenewer.new
    end

    ##
    # Advertises this service on the primary remote TupleSpace.

    def provide
      ts = Rinda::RingFinger.primary
      ts.write(@tuple, @renewer)
    end

  end

end

if __FILE__ == $0
  DRb.start_service
  case ARGV.shift
  when 's'
    require 'rinda/tuplespace'
    ts = Rinda::TupleSpace.new
    Rinda::RingServer.new(ts)
    $stdin.gets
  when 'w'
    finger = Rinda::RingFinger.new(nil)
    finger.lookup_ring do |ts2|
      p ts2
      ts2.write([:hello, :world])
    end
  when 'r'
    finger = Rinda::RingFinger.new(nil)
    finger.lookup_ring do |ts2|
      p ts2
      p ts2.take([nil, nil])
    end
  end
end

