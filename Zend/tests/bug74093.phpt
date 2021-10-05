--TEST--
Bug #74093 (Maximum execution time exceeded not written in error_log)
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
if (PHP_ZTS) die("skip only for no-zts build");
if (substr(PHP_OS, 0, 3) == 'WIN') die("skip not for Windows");
?>
--INI--
memory_limit=1G
max_execution_time=1
hard_timeout=1
--FILE--
<?php
$start = time();
while (time() - $start < 5);
die("Failed to interrupt execution");
?>
--EXPECTF--
Fatal error: Maximum execution time of 1 second exceeded in %s
