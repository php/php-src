--TEST--
Interactive shell: output through cli.pager
--EXTENSIONS--
readline
--SKIPIF--
<?php
if (!function_exists('proc_open')) die('skip proc_open() not available');
if (READLINE_LIB !== "readline") die('skip readline only');
if (PHP_OS_FAMILY === 'Windows') die('skip tr pager is not portable on Windows');
?>
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
$ini = getenv('TEST_PHP_EXTRA_ARGS');
$descriptorspec = [['pipe', 'r'], STDOUT, STDERR];
$proc = proc_open("$php $ini -d cli.pager='tr a-z A-Z' -a", $descriptorspec, $pipes);
fwrite($pipes[0], "echo \"pager output\n\";\n");
fwrite($pipes[0], "quit\n");
fclose($pipes[0]);
proc_close($proc);
?>
--EXPECT--
Interactive shell

php > echo "pager output
php " ";
pager output
PAGER OUTPUT
php > quit
