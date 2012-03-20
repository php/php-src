require 'drb/drb'
require 'monitor'

module DRb
  class TimerIdConv < DRbIdConv
    class TimerHolder2
      include MonitorMixin

      class InvalidIndexError < RuntimeError; end

      def initialize(timeout=600)
        super()
        @sentinel = Object.new
        @gc = {}
        @curr = {}
        @renew = {}
        @timeout = timeout
        @keeper = keeper
      end

      def add(obj)
        synchronize do
          key = obj.__id__
          @curr[key] = obj
          return key
        end
      end

      def fetch(key, dv=@sentinel)
        synchronize do
          obj = peek(key)
          if obj == @sentinel
            return dv unless dv == @sentinel
            raise InvalidIndexError
          end
          @renew[key] = obj # KeepIt
          return obj
        end
      end

      def include?(key)
        synchronize do
          obj = peek(key)
          return false if obj == @sentinel
          true
        end
      end

      def peek(key)
        synchronize do
          return @curr.fetch(key, @renew.fetch(key, @gc.fetch(key, @sentinel)))
        end
      end

      private
      def alternate
        synchronize do
          @gc = @curr       # GCed
          @curr = @renew
          @renew = {}
        end
      end

      def keeper
        Thread.new do
          loop do
            alternate
            sleep(@timeout)
          end
        end
      end
    end

    def initialize(timeout=600)
      @holder = TimerHolder2.new(timeout)
    end

    def to_obj(ref)
      return super if ref.nil?
      @holder.fetch(ref)
    rescue TimerHolder2::InvalidIndexError
      raise "invalid reference"
    end

    def to_id(obj)
      return @holder.add(obj)
    end
  end
end

# DRb.install_id_conv(TimerIdConv.new)
