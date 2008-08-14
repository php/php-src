--TEST--
using invalid combinations of cmdline options
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE');

var_dump(`$php -n -a -r "echo hello;"`);
var_dump(`$php -n -r "echo hello;" -a`);

echo "Done\n";
?>
--EXPECTF--	
Either execute direct code, process stdin or use a file.
NULL
Either execute direct code, process stdin or use a file.
NULL
Done
