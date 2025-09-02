--TEST--
using invalid combinations of cmdline options
--EXTENSIONS--
readline
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');

var_dump(shell_exec(<<<SHELL
$php -n -a -r "echo hello;"
SHELL));
var_dump(shell_exec(<<<SHELL
$php -n -r "echo hello;" -a
SHELL));

echo "Done\n";
?>
--EXPECT--
string(57) "Either execute direct code, process stdin or use a file.
"
string(57) "Either execute direct code, process stdin or use a file.
"
Done
