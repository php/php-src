--TEST--
using invalid combinations of cmdline options
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE');

var_dump(`$php -n -c -r "echo hello;"`);
var_dump(`$php -n -a -r "echo hello;"`);
var_dump(`$php -n -r "echo hello;" -a`);

echo "Done\n";
?>
--EXPECTF--	
You cannot use both -n and -c switch. Use -h for help.
NULL
Either execute direct code, process stdin or use a file.
NULL
Either execute direct code, process stdin or use a file.
NULL
Done
