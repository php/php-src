unless defined?(::JSON::JSON_LOADED) and ::JSON::JSON_LOADED
  require 'json'
end
defined?(::Complex) or require 'complex'

class Complex
  def self.json_create(object)
    Complex(object['r'], object['i'])
  end

  def as_json(*)
    {
      JSON.create_id => self.class.name,
      'r'            => real,
      'i'            => imag,
    }
  end

  def to_json(*)
    as_json.to_json
  end
end
