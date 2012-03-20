unless defined?(::JSON::JSON_LOADED) and ::JSON::JSON_LOADED
  require 'json'
end
defined?(::Rational) or require 'rational'

class Rational
  def self.json_create(object)
    Rational(object['n'], object['d'])
  end

  def as_json(*)
    {
      JSON.create_id => self.class.name,
      'n'            => numerator,
      'd'            => denominator,
    }
  end

  def to_json(*)
    as_json.to_json
  end
end
