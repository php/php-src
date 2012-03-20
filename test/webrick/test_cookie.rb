require "test/unit"
require "webrick/cookie"

class TestWEBrickCookie < Test::Unit::TestCase
  def test_new
    cookie = WEBrick::Cookie.new("foo","bar")
    assert_equal("foo", cookie.name)
    assert_equal("bar", cookie.value)
    assert_equal("foo=bar", cookie.to_s)
  end

  def test_time
    cookie = WEBrick::Cookie.new("foo","bar")
    t = 1000000000
    cookie.max_age = t
    assert_match(t.to_s, cookie.to_s)

    cookie = WEBrick::Cookie.new("foo","bar")
    t = Time.at(1000000000)
    cookie.expires = t
    assert_equal(Time, cookie.expires.class)
    assert_equal(t, cookie.expires)
    ts = t.httpdate
    cookie.expires = ts
    assert_equal(Time, cookie.expires.class)
    assert_equal(t, cookie.expires)
    assert_match(ts, cookie.to_s)
  end

  def test_parse
    data = ""
    data << '$Version="1"; '
    data << 'Customer="WILE_E_COYOTE"; $Path="/acme"; '
    data << 'Part_Number="Rocket_Launcher_0001"; $Path="/acme"; '
    data << 'Shipping="FedEx"; $Path="/acme"'
    cookies = WEBrick::Cookie.parse(data)
    assert_equal(3, cookies.size)
    assert_equal(1, cookies[0].version)
    assert_equal("Customer", cookies[0].name)
    assert_equal("WILE_E_COYOTE", cookies[0].value)
    assert_equal("/acme", cookies[0].path)
    assert_equal(1, cookies[1].version)
    assert_equal("Part_Number", cookies[1].name)
    assert_equal("Rocket_Launcher_0001", cookies[1].value)
    assert_equal(1, cookies[2].version)
    assert_equal("Shipping", cookies[2].name)
    assert_equal("FedEx", cookies[2].value)

    data = "hoge=moge; __div__session=9865ecfd514be7f7"
    cookies = WEBrick::Cookie.parse(data)
    assert_equal(0, cookies[0].version)
    assert_equal("hoge", cookies[0].name)
    assert_equal("moge", cookies[0].value)
    assert_equal("__div__session", cookies[1].name)
    assert_equal("9865ecfd514be7f7", cookies[1].value)
  end

  def test_parse_no_whitespace
    data = [
      '$Version="1"; ',
      'Customer="WILE_E_COYOTE";$Path="/acme";', # no SP between cookie-string
      'Part_Number="Rocket_Launcher_0001";$Path="/acme";', # no SP between cookie-string
      'Shipping="FedEx";$Path="/acme"'
    ].join
    cookies = WEBrick::Cookie.parse(data)
    assert_equal(1, cookies.size)
  end

  def test_parse_too_much_whitespaces
    # According to RFC6265,
    #   cookie-string = cookie-pair *( ";" SP cookie-pair )
    # So single 0x20 is needed after ';'. We allow multiple spaces here for
    # compatibility with older WEBrick versions.
    data = [
      '$Version="1"; ',
      'Customer="WILE_E_COYOTE";$Path="/acme";     ', # no SP between cookie-string
      'Part_Number="Rocket_Launcher_0001";$Path="/acme";     ', # no SP between cookie-string
      'Shipping="FedEx";$Path="/acme"'
    ].join
    cookies = WEBrick::Cookie.parse(data)
    assert_equal(3, cookies.size)
  end

  def test_parse_set_cookie
    data = %(Customer="WILE_E_COYOTE"; Version="1"; Path="/acme")
    cookie = WEBrick::Cookie.parse_set_cookie(data)
    assert_equal("Customer", cookie.name)
    assert_equal("WILE_E_COYOTE", cookie.value)
    assert_equal(1, cookie.version)
    assert_equal("/acme", cookie.path)

    data = %(Shipping="FedEx"; Version="1"; Path="/acme"; Secure)
    cookie = WEBrick::Cookie.parse_set_cookie(data)
    assert_equal("Shipping", cookie.name)
    assert_equal("FedEx", cookie.value)
    assert_equal(1, cookie.version)
    assert_equal("/acme", cookie.path)
    assert_equal(true, cookie.secure)
  end

  def test_parse_set_cookies
    data = %(Shipping="FedEx"; Version="1"; Path="/acme"; Secure)
    data << %(, CUSTOMER=WILE_E_COYOTE; path=/; expires=Wednesday, 09-Nov-99 23:12:40 GMT; path=/; Secure)
    data << %(, name="Aaron"; Version="1"; path="/acme")
    cookies = WEBrick::Cookie.parse_set_cookies(data)
    assert_equal(3, cookies.length)

    fed_ex = cookies.find { |c| c.name == 'Shipping' }
    assert_not_nil(fed_ex)
    assert_equal("Shipping", fed_ex.name)
    assert_equal("FedEx", fed_ex.value)
    assert_equal(1, fed_ex.version)
    assert_equal("/acme", fed_ex.path)
    assert_equal(true, fed_ex.secure)

    name = cookies.find { |c| c.name == 'name' }
    assert_not_nil(name)
    assert_equal("name", name.name)
    assert_equal("Aaron", name.value)
    assert_equal(1, name.version)
    assert_equal("/acme", name.path)

    customer = cookies.find { |c| c.name == 'CUSTOMER' }
    assert_not_nil(customer)
    assert_equal("CUSTOMER", customer.name)
    assert_equal("WILE_E_COYOTE", customer.value)
    assert_equal(0, customer.version)
    assert_equal("/", customer.path)
    assert_equal(Time.utc(1999, 11, 9, 23, 12, 40), customer.expires)
  end
end
