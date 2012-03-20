unless defined?(::JSON::JSON_LOADED) and ::JSON::JSON_LOADED
  require 'json'
end
defined?(::BigDecimal) or require 'bigdecimal'

class BigDecimal
  def self.json_create(object)
    BigDecimal._load object['b']
  end

  def as_json(*)
    {
      JSON.create_id => self.class.name,
      'b'            => _dump,
    }
  end

  def to_json(*)
    as_json.to_json
  end
end
