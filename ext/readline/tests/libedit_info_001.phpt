--TEST--
readline_info(): Basic test
--EXTENSIONS--
readline
--SKIPIF--
<?php if (READLINE_LIB != "libedit") die("skip libedit only");
if(substr(PHP_OS, 0, 3) == 'WIN' ) {
    die('skip not for windows');
}
if (getenv('SKIP_REPEAT')) die("skip readline has global state");
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
array(6) {
  ["line_buffer"]=>
  string(0) ""
  ["point"]=>
  int(0)
  ["end"]=>
  int(0)
  ["library_version"]=>
  string(%d) "%s"
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
