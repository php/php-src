--TEST--
GH-17428 (Assertion failure ext/opcache/jit/zend_jit_ir.c:8940)
--EXTENSIONS--
opcache
--INI--
opcache.jit=1205
--FILE--
<?php
new EmptyIterator();
srand(1000);
error_reporting(E_ALL);
testConversion('', '');
testConversion('', '');
testConversion('', '');
testConversion('', '');
testConversion('', '');
function testRoundTrip($data) {
}
for ($iterations = 0; $iterations < 100; $iterations++) {
    $strlen = rand(1, 100);
    $characters = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
    $randstring = '';
    for ($i = 0; $i < $strlen; $i++) {
        $randstring .= $characters[rand(0, strlen($characters) - 1)];
    }
    die($randstring);
}
echo "Done!\n";
throw new Hello(new stdClass);
?>
--EXPECTF--
Fatal error: Uncaught Error: Call to undefined function testConversion() in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
