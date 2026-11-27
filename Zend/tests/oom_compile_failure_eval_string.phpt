--TEST--
OOM: Delayed OOM raised during compilation of php -r code is reported when compilation fails
--SKIPIF--
<?php
if (!getenv('TEST_PHP_EXECUTABLE')) die('skip TEST_PHP_EXECUTABLE is not set');
if (PHP_OS_FAMILY === 'Windows') die('skip the -r code is longer than Windows allows on the command line');
?>
--FILE--
<?php

$code = '$a = [' . str_repeat('1,', 50000) . ']; )';

$php = getenv('TEST_PHP_EXECUTABLE');
echo shell_exec(escapeshellarg($php) . ' -n -d fatal_error_backtraces=0 -d memory_limit=3M -r ' . escapeshellarg($code) . ' 2>&1');

?>
--EXPECTF--
Fatal error: Allowed memory size of 3145728 bytes exhausted %S(tried to allocate %d bytes) in Command line code on line 1
