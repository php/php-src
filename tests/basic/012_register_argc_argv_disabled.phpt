--TEST--
Testing $argc and $argv handling (cli, register_argc_argv=0)
--INI--
register_argc_argv=0
variables_order=GPS
--ARGS--
ab cd ef 123 test
--FILE--
<?php

var_dump(
    $argc,
    $argv,
    $_SERVER['argc'],
    $_SERVER['argv'],
);

?>
--EXPECTF--
int(6)
array(6) {
  [0]=>
  string(%d) "%s"
  [1]=>
  string(2) "ab"
  [2]=>
  string(2) "cd"
  [3]=>
  string(2) "ef"
  [4]=>
  string(3) "123"
  [5]=>
  string(4) "test"
}
int(6)
array(6) {
  [0]=>
  string(%d) "%s"
  [1]=>
  string(2) "ab"
  [2]=>
  string(2) "cd"
  [3]=>
  string(2) "ef"
  [4]=>
  string(3) "123"
  [5]=>
  string(4) "test"
}
