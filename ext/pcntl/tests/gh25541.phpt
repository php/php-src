--TEST--
GH-25541 (pcntl handler for a signal delivered on a thread that never ran PHP)
--EXTENSIONS--
pcntl
zend_test
--SKIPIF--
<?php
if (!PHP_ZTS) die('skip ZTS only');
if (!function_exists('zend_test_raise_on_foreign_thread')) die('skip zend_test_raise_on_foreign_thread() not available');
if (ini_get('zend.signal_check') !== false) die('skip pcntl handlers are behind zend signals');
?>
--FILE--
<?php
$handled = false;
pcntl_signal(SIGUSR1, function (int $signo) use (&$handled) {
    $handled = true;
    echo "handled SIGUSR1\n";
});

zend_test_raise_on_foreign_thread(SIGUSR1);

for ($i = 0; !$handled && $i < 100; $i++) {
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
