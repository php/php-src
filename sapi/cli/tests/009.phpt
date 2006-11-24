--TEST--
using invalid combinations of cmdline options
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

$php = $_ENV['TEST_PHP_EXECUTABLE'];

var_dump(`"$php" -n -c -r 'echo hello;'`);
var_dump(`"$php" -a -r 'echo hello;'`);
var_dump(`"$php" -r 'echo hello;' -a`);

echo "Done\n";
?>
--EXPECTF--	
string(55) "You cannot use both -n and -c switch. Use -h for help.
"
string(57) "Either execute direct code, process stdin or use a file.
"
string(57) "Either execute direct code, process stdin or use a file.
"
Done
