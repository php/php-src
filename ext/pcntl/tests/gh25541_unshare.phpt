--TEST--
GH-25541 (pcntl_unshare() still works, and forwarding goes on, with a signal handler)
--EXTENSIONS--
pcntl
zend_test
--SKIPIF--
<?php
if (!PHP_ZTS) die('skip ZTS only');
if (!function_exists('pcntl_unshare')) die('skip pcntl_unshare() is not available');
if (!defined('CLONE_NEWUSER')) die('skip flag unavailable');
if (!function_exists('zend_test_raise_on_foreign_thread')) die('skip zend_test_raise_on_foreign_thread() not available');
if (!zend_test_signal_forwarding('supported')) die('skip signal forwarding is not supported on this platform');
if (@pcntl_unshare(CLONE_NEWUSER) == false && pcntl_get_last_error() == PCNTL_EPERM) {
    die('skip Insufficient privileges to use CLONE_NEWUSER');
}
if (getenv('SKIP_REPEAT')) die('skip cannot be repeated: the process is in a new user namespace');
?>
--FILE--
<?php
$handled = false;
pcntl_signal(SIGUSR1, function () use (&$handled) {
    $handled = true;
});

var_dump(pcntl_unshare(CLONE_NEWUSER));

zend_test_raise_on_foreign_thread(SIGUSR1);
for ($i = 0; !$handled && $i < 3000; $i++) {
    pcntl_signal_dispatch();
    usleep(10000);
}
var_dump($handled);
?>
--EXPECT--
bool(true)
bool(true)
