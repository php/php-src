--TEST--
A signal interrupts sleep(), usleep(), time_nanosleep() and stream_socket_accept()
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (!function_exists('time_nanosleep')) die('skip no time_nanosleep');
?>
--FILE--
<?php
pcntl_async_signals(true);
pcntl_signal(SIGALRM, function () { echo "handler\n"; });

pcntl_alarm(1);
$r = sleep(10);
var_dump($r > 0 && $r < 10);

pcntl_alarm(1);
$t = microtime(true);
usleep(10000000);
var_dump(microtime(true) - $t < 9);

pcntl_alarm(1);
$r = time_nanosleep(10, 0);
var_dump($r['seconds'] < 10, $r['nanoseconds'] >= 0);

/* The seconds do not overflow into a short sleep */
pcntl_alarm(1);
$r = time_nanosleep(PHP_INT_MAX, 0);
var_dump($r['seconds'] > PHP_INT_MAX - 10);

$s = stream_socket_server("tcp://127.0.0.1:0");
pcntl_alarm(1);
var_dump(stream_socket_accept($s, -1));
?>
--EXPECTF--
handler
bool(true)
handler
bool(true)
handler
bool(true)
bool(true)
handler
bool(true)

Warning: stream_socket_accept(): Accept failed: Interrupted system call in %s on line %d
handler
bool(false)
