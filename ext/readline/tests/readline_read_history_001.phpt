--TEST--
readline_read_history(): Basic test
--EXTENSIONS--
readline
--SKIPIF--
<?php
if (!function_exists('readline_list_history')) die("skip");
if (getenv('SKIP_REPEAT')) die("skip readline has global state");
?>
--FILE--
<?php

$name = tempnam(sys_get_temp_dir(), 'readline.tmp');

readline_add_history("foo");

var_dump(readline_write_history($name));

var_dump(readline_clear_history());

var_dump(readline_read_history($name));

var_dump(readline_list_history());

unlink($name);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
array(1) {
  [0]=>
  string(3) "foo"
}
