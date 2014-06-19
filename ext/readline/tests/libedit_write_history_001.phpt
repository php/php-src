--TEST--
readline_write_history(): Basic test
--SKIPIF--
<?php if (!extension_loaded("readline") || !function_exists('readline_add_history')) die("skip");
if (READLINE_LIB != "libedit") die("skip libedit only");
?>
--FILE--
<?php

$name = tempnam('/tmp', 'readline.tmp');

readline_add_history('foo');
readline_add_history('');
readline_add_history(1);
readline_add_history(NULL);
readline_write_history($name);

var_dump(file_get_contents($name));

unlink($name);

?>
--EXPECT--
string(21) "_HiStOrY_V2_
foo

1

"
