--TEST--
readline_info(): Basic test
--SKIPIF--
<?php if (!extension_loaded("readline")) die("skip"); ?>
--FILE--
<?php

var_dump(readline_info());
var_dump(readline_info(1));
var_dump(readline_info(1,1));
var_dump(readline_info('line_buffer'));
var_dump(readline_info('readline_name'));
var_dump(readline_info('readline_name', 1));
var_dump(readline_info('readline_name'));

?>
--EXPECTF--
array(10) {
  ["line_buffer"]=>
  string(0) ""
  ["point"]=>
  int(0)
  ["end"]=>
  int(0)
  ["mark"]=>
  int(0)
  ["done"]=>
  int(0)
  ["pending_input"]=>
  int(0)
  ["prompt"]=>
  string(0) ""
  ["terminal_name"]=>
  string(0) ""
  ["library_version"]=>
  string(%d) "%s"
  ["readline_name"]=>
  string(5) "other"
}
NULL
NULL
string(0) ""
string(5) "other"
string(5) "other"
string(1) "1"
