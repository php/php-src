--TEST--
readline_read_history(): Basic test
--SKIPIF--
<?php if (!extension_loaded("readline") || !function_exists('readline_list_history')) die("skip"); ?>
--FILE--
<?php

$name = tempnam('/tmp', 'readline.tmp');

readline_add_history("foo\n");

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
