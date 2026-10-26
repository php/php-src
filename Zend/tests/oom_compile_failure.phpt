--TEST--
OOM: Delayed OOM raised during compilation of the main script is reported when compilation fails
--SKIPIF--
<?php
if (!getenv('TEST_PHP_EXECUTABLE')) die('skip TEST_PHP_EXECUTABLE is not set');
?>
--FILE--
<?php

$fname = __DIR__ . '/oom_compile_failure.test.php';
file_put_contents($fname, "<?php\n\$a = '" . str_repeat('A', 3 << 20) . "';\n)\n");

$php = getenv('TEST_PHP_EXECUTABLE');
echo shell_exec(escapeshellarg($php) . ' -n -d fatal_error_backtraces=0 -d memory_limit=6M ' . escapeshellarg($fname) . ' 2>&1');

?>
--CLEAN--
<?php
unlink(__DIR__ . '/oom_compile_failure.test.php');
?>
--EXPECTF--
Fatal error: Allowed memory size of 6291456 bytes exhausted %S(tried to allocate %d bytes) in %soom_compile_failure.test.php on line 2
