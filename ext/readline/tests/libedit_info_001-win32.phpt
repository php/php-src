--TEST--
readline_info(): Basic test
--SKIPIF--
<?php if (!extension_loaded("readline")) die("skip");
if (READLINE_LIB != "libedit") die("skip libedit only");
if(substr(PHP_OS, 0, 3) != 'WIN' ) {
	die('skip windows only test');
}
?>
--FILE--
<?php

var_dump(readline_info());
var_dump(readline_info(1));
var_dump(readline_info(1,1));
var_dump(readline_info('line_buffer'));
var_dump(readline_info('readline_name'));
var_dump(readline_info('readline_name', 1));
var_dump(readline_info('readline_name'));
var_dump(readline_info('attempted_completion_over',1));
var_dump(readline_info('attempted_completion_over'));

?>
--EXPECTF--
array(4) {
  ["line_buffer"]=>
  string(0) ""
  ["point"]=>
  int(0)
  ["readline_name"]=>
  string(0) ""
  ["attempted_completion_over"]=>
  int(0)
}
NULL
NULL
string(0) ""
string(0) ""
string(0) ""
string(1) "1"
int(0)
int(1)
