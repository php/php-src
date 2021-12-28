--TEST--
Test parse_url() function: The url is 127.0.0.1:9999?
--FILE--
<?php
echo "*** Testing parse_url() :The url is 127.0.0.1:9999? ***\n";
$url = '127.0.0.1:9999?';

var_dump(parse_url($url));
echo "Done"
?>
--EXPECT--
*** Testing parse_url() :The url is 127.0.0.1:9999? ***
array(3) {
  ["host"]=>
  string(9) "127.0.0.1"
  ["port"]=>
  int(9999)
  ["query"]=>
  string(0) ""
}
Done
