--TEST--
more invalid arguments and error messages
--EXTENSIONS--
readline
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE');

// -r : behavior = CLI_DIRECT
// -F / -R / -B / -E : behavior = PROCESS_STDIN
// -l : behavior = LINT
// -s : behavior = HIGHLIGHT
// -w : behavior = STRIP

var_dump(`"$php" -n -r "echo 1;" -F some.php`);
var_dump(`"$php" -n -r "echo 2;" -f some.php`);
var_dump(`"$php" -n -r "echo 3;" -l`); // ignores linting
var_dump(`"$php" -n -r "echo 4;" -R some.php`);
var_dump(`"$php" -n -r "echo 5;" -B ""`);
var_dump(`"$php" -n -a -B ""`);
var_dump(`"$php" -n -r "echo 6;" -E ""`);
var_dump(`"$php" -n -a -E ""`);
var_dump(`"$php" -n -r "echo 7;" -s`);
var_dump(`"$php" -n -r "echo 8;" -w`);
var_dump(`"$php" -n -l -r "echo 9;"`);

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
