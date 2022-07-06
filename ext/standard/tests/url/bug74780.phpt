--TEST--
Bug #74780	parse_url() borks when query string contains colon
--FILE--
<?php
var_dump(
    parse_url('//php.net/path?query=1:2'),
    parse_url('//php.net/path.php?query=a:b'),
    parse_url('//username@php.net/path?query=1:2')
);

?>
--EXPECT--
array(3) {
  ["host"]=>
  string(7) "php.net"
  ["path"]=>
  string(5) "/path"
  ["query"]=>
  string(9) "query=1:2"
}
array(3) {
  ["host"]=>
  string(7) "php.net"
  ["path"]=>
  string(9) "/path.php"
  ["query"]=>
  string(9) "query=a:b"
}
array(4) {
  ["host"]=>
  string(7) "php.net"
  ["user"]=>
  string(8) "username"
  ["path"]=>
  string(5) "/path"
  ["query"]=>
  string(9) "query=1:2"
}
