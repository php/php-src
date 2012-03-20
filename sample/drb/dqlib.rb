class DQEntry
  def initialize(name)
    @name = name
  end

  def greeting
    "Hello, This is #{@name}."
  end
  alias to_s greeting
end

if __FILE__ == $0
  puts DQEntry.new('DQEntry')
end
