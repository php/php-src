require 'dl.so'

begin
  require 'fiddle' unless Object.const_defined?(:Fiddle)
rescue LoadError
end

module DL
  def self.fiddle?
    Object.const_defined?(:Fiddle)
  end
end
