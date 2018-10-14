--TEST--
running code with -r
--SKIPIF--
<?php
include "skipif.inc";
if (substr(PHP_OS, 0, 3) != 'WIN') {
	die ("skip only for Windows");
}
?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE');

var_dump(`$php -n -r "var_dump('hello');"`);

echo "Done\n";
?>
--EXPECTF--
string(18) "string(5) "hello"
"
Done
