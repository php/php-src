--TEST--
Using headers_send_early_and_clear() for HTTP early hints (after output)
--CGI--
--FILE--
<?php

echo "Foo\n";
var_dump(headers_send_early_and_clear());
?>
--EXPECTF--
Foo

Warning: headers_send_early_and_clear(): Headers already sent in %s on line %d
bool(false)
