require 'rinda/ring'
require 'drb/drb'

class Inspector
  def initialize
  end

  def primary
    Rinda::RingFinger.primary
  end

  def list_place
    Rinda::RingFinger.to_a
  end

  def list(idx = -1)
    if idx < 0
      ts = primary
    else
      ts = list_place[idx]
      raise "RingNotFound" unless ts
    end
    ts.read_all([:name, nil, nil, nil])
  end
end

def main
  DRb.start_service
  r = Inspector.new
end
