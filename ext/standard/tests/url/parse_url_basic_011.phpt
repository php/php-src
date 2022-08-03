--TEST--
Test parse_url() function: can not recognize port without scheme
--FILE--
<?php
echo "*** Testing parse_url() :can not recognize port without scheme ***\n";
echo 'parse 127.0.0.1:9999?', PHP_EOL;
var_dump(parse_url('127.0.0.1:9999?'));
echo 'parse 127.0.0.1:9999#', PHP_EOL;
var_dump(parse_url('127.0.0.1:9999#'));
?>
--EXPECT--
*** Testing parse_url() :can not recognize port without scheme ***
parse 127.0.0.1:9999?
array(3) {
  ["host"]=>
  string(9) "127.0.0.1"
  ["port"]=>
  int(9999)
  ["query"]=>
  string(0) ""
}
parse 127.0.0.1:9999#
array(3) {
  ["host"]=>
  string(9) "127.0.0.1"
  ["port"]=>
  int(9999)
  ["fragment"]=>
  string(0) ""
}
