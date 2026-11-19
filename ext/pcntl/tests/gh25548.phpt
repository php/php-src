--TEST--
GH-25548 (execution timeout delivered to a thread that never ran PHP)
--EXTENSIONS--
pcntl
zend_test
--SKIPIF--
<?php
if (!PHP_ZTS) die('skip ZTS only');
if (!function_exists('zend_test_timeout_signal') || zend_test_timeout_signal() === null) die('skip the execution timer is not process-wide');
if (getenv('SKIP_SLOW_TESTS')) die('skip slow test');
if (!zend_test_signal_forwarding('supported')) die('skip signal forwarding is not supported on this platform');
?>
--FILE--
<?php
set_time_limit(1);
zend_test_raise_on_foreign_thread(zend_test_timeout_signal());
$end = hrtime(true) + 10_000_000_000;
while (hrtime(true) < $end);
echo "no timeout\n";
?>
--EXPECTF--
Fatal error: Maximum execution time of 1 second exceeded in %s on line %d
