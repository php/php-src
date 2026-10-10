--TEST--
A read on a pipe retries once after EINTR and then gives up
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (!function_exists('proc_open')) die('skip no proc_open');
?>
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
$args = getenv('TEST_PHP_EXTRA_ARGS');

$code = <<<'CODE'
pcntl_async_signals(true);
pcntl_signal(SIGUSR1, function () { echo "signal\n"; }, false);
echo "ready\n";
var_dump(fgets(STDIN));
CODE;
/* exec: the signals go to PHP, not to the shell */
$p = proc_open("exec $php $args -r " . escapeshellarg($code), [0 => ['pipe', 'r'], 1 => ['pipe', 'w']], $pipes);
echo fgets($pipes[1]);
usleep(300000);
proc_terminate($p, SIGUSR1);
usleep(300000);
proc_terminate($p, SIGUSR1);
usleep(300000);
/* Unblocks the child if it still reads */
@fwrite($pipes[0], "data\n");
echo stream_get_contents($pipes[1]);
proc_close($p);
?>
--EXPECT--
ready
signal
signal
bool(false)
