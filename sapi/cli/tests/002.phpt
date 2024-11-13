--TEST--
running code with -r
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');

var_dump(`$php -n -r "var_dump('hello');"`);

echo "Done\n";
?>
--EXPECT--
string(18) "string(5) "hello"
"
Done
