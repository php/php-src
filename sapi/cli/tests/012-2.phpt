--TEST--
more invalid arguments and error messages
--EXTENSIONS--
readline
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');

// -r : behavior = CLI_DIRECT
// -F / -R / -B / -E : behavior = PROCESS_STDIN
// -l : behavior = LINT
// -s : behavior = HIGHLIGHT
// -w : behavior = STRIP

var_dump(shell_exec(<<<SHELL
$php -n -r "echo 1;" -F some.php
SHELL));
var_dump(shell_exec(<<<SHELL
$php -n -r "echo 2;" -f some.php
SHELL));
var_dump(shell_exec(<<<SHELL
$php -n -r "echo 3;" -l
SHELL)); // ignores linting
var_dump(shell_exec(<<<SHELL
$php -n -r "echo 4;" -R some.php
SHELL));
var_dump(shell_exec(<<<SHELL
$php -n -r "echo 5;" -B ""
SHELL));
var_dump(shell_exec(<<<SHELL
$php -n -a -B ""
SHELL));
var_dump(shell_exec(<<<SHELL
$php -n -r "echo 6;" -E ""
SHELL));
var_dump(shell_exec(<<<SHELL
$php -n -a -E ""
SHELL));
var_dump(shell_exec(<<<SHELL
$php -n -r "echo 7;" -s
SHELL));
var_dump(shell_exec(<<<SHELL
$php -n -r "echo 8;" -w
SHELL));
var_dump(shell_exec(<<<SHELL
$php -n -l -r "echo 9;"
SHELL));

echo "Done\n";
?>
--EXPECT--
string(57) "Either execute direct code, process stdin or use a file.
"
string(57) "Either execute direct code, process stdin or use a file.
"
string(1) "3"
string(57) "Either execute direct code, process stdin or use a file.
"
string(57) "Either execute direct code, process stdin or use a file.
"
string(57) "Either execute direct code, process stdin or use a file.
"
string(57) "Either execute direct code, process stdin or use a file.
"
string(57) "Either execute direct code, process stdin or use a file.
"
string(42) "Source highlighting only works for files.
"
string(39) "Source stripping only works for files.
"
string(57) "Either execute direct code, process stdin or use a file.
"
Done
