--TEST--
GH-21018 (header() removes headers with the same prefix)
--INI--
expose_php=On
--CGI--
--FILE--
<?php
header('a: 1');
header('a-test: 1');
header('a: 1');
var_dump(headers_list());
?>
--EXPECTF--
array(3) {
  [0]=>
  string(%d) "X-Powered-By: PHP/%s"
  [1]=>
  string(9) "a-test: 1"
  [2]=>
  string(4) "a: 1"
}
