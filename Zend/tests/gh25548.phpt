--TEST--
GH-25548 (Execution timeout delivered to a thread that never ran PHP)
--EXTENSIONS--
zend_test
--SKIPIF--
<?php
if (!function_exists('zend_test_timeout_signal')) die('skip ZTS only');
if (zend_test_timeout_signal() === null) die('skip the execution timer is not process-wide');
?>
--FILE--
<?php
set_time_limit(30);
zend_test_raise_on_foreign_thread(zend_test_timeout_signal());
// Only the re-routed signal can end this loop: the timer itself fires after 30s.
$end = hrtime(true) + 3_000_000_000;
while (hrtime(true) < $end);
echo "no timeout\n";
?>
--EXPECTF--
Fatal error: Maximum execution time of 30 seconds exceeded in %s on line %d
