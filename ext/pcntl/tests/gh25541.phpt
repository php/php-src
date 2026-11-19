--TEST--
GH-25541 (pcntl handler for a signal delivered on a thread that never ran PHP)
--EXTENSIONS--
pcntl
zend_test
--SKIPIF--
<?php
if (!PHP_ZTS) die('skip ZTS only');
if (!function_exists('zend_test_raise_on_foreign_thread')) die('skip zend_test_raise_on_foreign_thread() not available');
if (!zend_test_signal_forwarding('supported')) die('skip signal forwarding is not supported on this platform');
?>
--FILE--
<?php
$handled = false;
pcntl_signal(SIGUSR1, function (int $signo) use (&$handled) {
    $handled = true;
    echo "handled SIGUSR1\n";
});

zend_test_raise_on_foreign_thread(SIGUSR1);

for ($i = 0; !$handled && $i < 3000; $i++) {
    pcntl_signal_dispatch();
    if (!$handled) {
        usleep(10000);
    }
}
echo "done\n";
?>
--EXPECT--
handled SIGUSR1
done
