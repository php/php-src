--TEST--
Bug #74093 (Maximum execution time of n+2 seconds exceed not written in error_log)
--FLAKY--
--SKIPIF--
<?php
skipif(!getenv('RUN_RESOURCE_HEAVY_TESTS'), 'resource-heavy test');
skipif(getenv('SKIP_SLOW_TESTS'), 'slow test');
skipif(PHP_ZTS, 'only for no-zts build');
skipif(PHP_OS_FAMILY === 'Windows', 'not for Windows');
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
