--TEST--
GH-25541 (a fault on a thread that never ran PHP is not forwarded)
--EXTENSIONS--
pcntl
zend_test
--SKIPIF--
<?php
if (!PHP_ZTS) die('skip ZTS only');
if (!function_exists('zend_test_raise_on_foreign_thread')) die('skip zend_test_raise_on_foreign_thread() not available');
?>
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
$args = getenv('TEST_PHP_EXTRA_ARGS');

foreach (['SIGSEGV' => SIGSEGV, 'SIGTRAP' => SIGTRAP] as $name => $signo) {
    $code = <<<CODE
    pcntl_signal($signo, function () { echo "handled\\n"; });
    zend_test_raise_on_foreign_thread($signo);
    usleep(100000);
    pcntl_signal_dispatch();
    echo "survived\\n";
    CODE;

    $process = proc_open("ASAN_OPTIONS=handle_segv=0 exec $php $args -r " . escapeshellarg($code), [1 => ['pipe', 'w'], 2 => ['pipe', 'w']], $pipes);
    echo stream_get_contents($pipes[1]);
    do {
        $status = proc_get_status($process);
        usleep(10000);
    } while ($status['running']);
    echo $name, ': ', var_export($status['signaled'] && $status['termsig'] === $signo, true), "\n";
}
?>
--EXPECT--
SIGSEGV: true
SIGTRAP: true
