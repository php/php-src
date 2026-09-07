--TEST--
Bug #74093 (Maximum execution time of n+2 seconds exceed not written in error_log)
--FLAKY--
--SKIPIF--
<?php
if (!getenv('RUN_RESOURCE_HEAVY_TESTS')) die('skip resource-heavy test');
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
$values = range(1, 6000000);
/* array_intersect() now uses a linear-time hash implementation. Use a large
 * internal string sort to retain the hard-timeout workload. */
sort($values, SORT_STRING);
?>
--EXPECTF--
Fatal error: Maximum execution time of 1+1 seconds exceeded %s
