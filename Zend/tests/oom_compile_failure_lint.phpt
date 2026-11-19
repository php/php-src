--TEST--
OOM: Delayed OOM raised during compilation is reported by php -l
--SKIPIF--
<?php
if (!getenv('TEST_PHP_EXECUTABLE')) die('skip TEST_PHP_EXECUTABLE is not set');
?>
--FILE--
<?php

$fname = __DIR__ . '/oom_compile_failure_lint.test.php';
$php = getenv('TEST_PHP_EXECUTABLE');

foreach (["\n", "\n)\n"] as $tail) {
    file_put_contents($fname, "<?php\n\$a = '" . str_repeat('A', 3 << 20) . "';" . $tail);
    // Windows rounds huge blocks up to 2MB, so leave room below the hard limit
    echo shell_exec(escapeshellarg($php) . ' -n -d fatal_error_backtraces=0 -d memory_limit=6M -d memory_hard_limit_percent=400 -l ' . escapeshellarg($fname) . ' 2>&1');
}

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/oom_compile_failure_lint.test.php');
?>
--EXPECTF--
Fatal error: Allowed memory size of 6291456 bytes exhausted %S(tried to allocate %d bytes) in %soom_compile_failure_lint.test.php on line 2
Errors parsing %soom_compile_failure_lint.test.php

Fatal error: Allowed memory size of 6291456 bytes exhausted %S(tried to allocate %d bytes) in %soom_compile_failure_lint.test.php on line 2
Errors parsing %soom_compile_failure_lint.test.php
