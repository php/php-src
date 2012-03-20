require 'uri'

100_000.times{
  uri = URI.parse('http://www.ruby-lang.org')
  uri.scheme
  uri.host
  uri.port
}
