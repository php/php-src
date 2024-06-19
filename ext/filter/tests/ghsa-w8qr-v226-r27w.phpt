--TEST--
GHSA-w8qr-v226-r27w
--EXTENSIONS--
filter
--FILE--
<?php

function test(string $input) {
    var_dump(filter_var($input, FILTER_VALIDATE_URL));
}

echo "--- These ones should fail ---\n";
test("http://t[est@127.0.0.1");
test("http://t[est@[::1]");
test("http://t[est@[::1");
test("http://t[est@::1]");
test("http://php.net\\@aliyun.com/aaa.do");
test("http://test[@2001:db8:3333:4444:5555:6666:1.2.3.4]");
test("http://te[st@2001:db8:3333:4444:5555:6666:1.2.3.4]");
test("http://te[st@2001:db8:3333:4444:5555:6666:1.2.3.4");

echo "--- These ones should work ---\n";
test("http://test@127.0.0.1");
test("http://test@[2001:db8:3333:4444:5555:6666:1.2.3.4]");
test("http://test@[::1]");

?>
--EXPECT--
--- These ones should fail ---
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
--- These ones should work ---
string(21) "http://test@127.0.0.1"
string(50) "http://test@[2001:db8:3333:4444:5555:6666:1.2.3.4]"
string(17) "http://test@[::1]"
