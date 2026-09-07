--TEST--
Interactive shell: input buffer grows for long lines
--EXTENSIONS--
readline
--SKIPIF--
<?php
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
$ini = getenv('TEST_PHP_EXTRA_ARGS');
$descriptorspec = [['pipe', 'r'], STDOUT, STDERR];
$proc = proc_open("$php $ini -a", $descriptorspec, $pipes);
$long = str_repeat('x', 5000);
fwrite($pipes[0], 'echo strlen("' . $long . '") . "\n";' . "\n");
fwrite($pipes[0], "exit\n");
fclose($pipes[0]);
proc_close($proc);
?>
--EXPECTF--
%AInteractive shell%A5000%A
