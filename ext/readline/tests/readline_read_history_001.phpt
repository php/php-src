--TEST--
readline_read_history(): Basic test
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
