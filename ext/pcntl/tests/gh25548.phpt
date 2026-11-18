--TEST--
GH-25548 (execution timeout delivered to a thread that never ran PHP)
--EXTENSIONS--
pcntl
zend_test
--SKIPIF--
<?php
if (!PHP_ZTS) die('skip ZTS only');
if (!function_exists('zend_test_raise_on_foreign_thread')) die('skip zend_test_raise_on_foreign_thread() not available');
if (PHP_OS_FAMILY !== 'Darwin' || php_uname('m') !== 'arm64') die('skip SIGALRM is only the timeout signal on macOS arm64');
if (getenv('SKIP_SLOW_TESTS')) die('skip slow test');
?>
--FILE--
<?php
set_time_limit(1);
zend_test_raise_on_foreign_thread(SIGALRM);
$end = hrtime(true) + 10_000_000_000;
while (hrtime(true) < $end);
echo "no timeout\n";
?>
--EXPECTF--
Fatal error: Maximum execution time of 1 second exceeded in %s on line %d
