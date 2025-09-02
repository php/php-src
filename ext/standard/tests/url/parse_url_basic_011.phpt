--TEST--
Test parse_url() function: can not recognize port without scheme
--FILE--
<?php
echo "*** Testing parse_url() :can not recognize port without scheme ***\n";
echo 'parse 127.0.0.1:9999?', PHP_EOL;
var_dump(parse_url('127.0.0.1:9999?'));
echo 'parse 127.0.0.1:9999#', PHP_EOL;
var_dump(parse_url('127.0.0.1:9999#'));
echo 'parse internal:#feeding', PHP_EOL;
var_dump(parse_url('internal:#feeding'));
echo 'parse magnet:?xt=urn:sha1:YNCKHTQCWBTRNJIV4WNAE52SJUQCZO5C', PHP_EOL;
var_dump(parse_url('magnet:?xt=urn:sha1:YNCKHTQCWBTRNJIV4WNAE52SJUQCZO5C'));
?>
--EXPECT--
*** Testing parse_url() :can not recognize port without scheme ***
parse 127.0.0.1:9999?
array(3) {
  ["scheme"]=>
  string(9) "127.0.0.1"
  ["path"]=>
  string(4) "9999"
  ["query"]=>
  string(0) ""
}
parse 127.0.0.1:9999#
array(3) {
  ["scheme"]=>
  string(9) "127.0.0.1"
  ["path"]=>
  string(4) "9999"
  ["fragment"]=>
  string(0) ""
}
parse internal:#feeding
array(2) {
  ["scheme"]=>
  string(8) "internal"
  ["fragment"]=>
  string(7) "feeding"
}
parse magnet:?xt=urn:sha1:YNCKHTQCWBTRNJIV4WNAE52SJUQCZO5C
array(2) {
  ["scheme"]=>
  string(6) "magnet"
  ["query"]=>
  string(44) "xt=urn:sha1:YNCKHTQCWBTRNJIV4WNAE52SJUQCZO5C"
}
