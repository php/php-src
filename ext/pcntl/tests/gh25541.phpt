--TEST--
GH-25541 (Signal handler not run for a signal delivered to a thread that never ran PHP)
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
$count = 0;
pcntl_signal(SIGUSR1, function (int $signo) use (&$count) {
    $count++;
});

for ($i = 1; $i <= 3; $i++) {
    zend_test_raise_on_foreign_thread(SIGUSR1);
    for ($wait = 0; $count < $i && $wait < 500; $wait++) {
        pcntl_signal_dispatch();
        usleep(10000);
    }
}
var_dump($count);
?>
--EXPECT--
int(3)
