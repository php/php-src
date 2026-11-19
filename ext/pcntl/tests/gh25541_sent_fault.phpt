--TEST--
GH-25541 (SIGTRAP sent with kill() to a thread that never ran PHP is not a fault)
--EXTENSIONS--
pcntl
zend_test
--SKIPIF--
<?php
if (!function_exists('zend_test_raise_on_foreign_thread')) die('skip ZTS only');
if (!in_array(PHP_OS, ['Linux', 'Darwin', 'FreeBSD'], true)) die('skip signals are not re-routed on this platform');
?>
--FILE--
<?php
$handled = false;
pcntl_signal(SIGTRAP, function () use (&$handled) {
    $handled = true;
});

zend_test_raise_on_foreign_thread(SIGTRAP);
for ($wait = 0; !$handled && $wait < 500; $wait++) {
    pcntl_signal_dispatch();
    usleep(10000);
}
var_dump($handled);
?>
--EXPECT--
bool(true)
