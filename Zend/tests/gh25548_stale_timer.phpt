--TEST--
GH-25548 (Signal of a disarmed execution timer delivered to a thread that never ran PHP)
--EXTENSIONS--
zend_test
--SKIPIF--
<?php
if (!function_exists('zend_test_timeout_signal')) die('skip ZTS only');
if (zend_test_timeout_signal() === null) die('skip the execution timer is not process-wide');
?>
--FILE--
<?php
set_time_limit(60);
set_time_limit(0);
zend_test_raise_on_foreign_thread(zend_test_timeout_signal());
usleep(100000);
echo "survived\n";
?>
--EXPECT--
survived
