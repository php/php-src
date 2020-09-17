--TEST--
Bug #80114 (parse_url does not accept URLs with port 0)
--FILE--
<?php
var_dump(parse_url('https://example.com:0/'));
?>
--EXPECT--
array(3) {
  ["scheme"]=>
  string(5) "https"
  ["host"]=>
  string(11) "example.com"
  ["path"]=>
  string(1) "/"
}
