--TEST--
readline_info(): Basic test
--EXTENSIONS--
readline
--SKIPIF--
<?php if (READLINE_LIB == "libedit") die("skip readline only");
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
var_dump(readline_info('completion_append_character', "\0"));
var_dump(readline_info('completion_append_character'));
var_dump(readline_info('completion_suppress_append', true));
var_dump(readline_info('completion_suppress_append'));

?>
--EXPECTF--
array(13) {
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
  ["completion_append_character"]=>
  string(1) " "
  ["completion_suppress_append"]=>
  bool(false)
  ["library_version"]=>
  string(%d) "%s"
  ["readline_name"]=>
  string(5) "other"
  ["attempted_completion_over"]=>
  int(0)
}
NULL
NULL
string(0) ""
string(5) "other"
string(5) "other"
string(1) "1"
int(0)
int(1)
string(1) " "
string(0) ""
bool(false)
bool(true)
