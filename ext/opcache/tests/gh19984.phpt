--TEST--
GH-19984: Double-free of EG(errors)/persistent_script->warnings on persist of already persisted file
--EXTENSIONS--
opcache
pcntl
--INI--
opcache.enable_cli=1
opcache.record_warnings=1
--SKIPIF--
<?php
if (!function_exists('pcntl_fork')) die('skip pcntl_fork() not available');
?>
--FILE--
<?php
$pid = pcntl_fork();
require __DIR__ . '/warning_replay.inc';
if ($pid) {
    // Wait for child to output its error message.
    pcntl_waitpid($pid, $status);
}
?>
--EXPECTF--
Warning: Unsupported declare 'unknown' in %s on line %d

Warning: Unsupported declare 'unknown' in %s on line %d
