--TEST--
OOM: Delayed OOM raised during compilation of php -r code is reported when compilation fails
--SKIPIF--
<?php
if (!getenv('TEST_PHP_EXECUTABLE')) die('skip TEST_PHP_EXECUTABLE is not set');
?>
--FILE--
<?php

$code = '$a = [' . str_repeat('1,', 60000) . ']; )';

$php = getenv('TEST_PHP_EXECUTABLE');
echo shell_exec(escapeshellarg($php) . ' -n -d memory_limit=4M -r ' . escapeshellarg($code) . ' 2>&1');

?>
--EXPECTF--
Fatal error: Allowed memory size of 4194304 bytes exhausted %S(tried to allocate %d bytes) in Command line code on line 1
