--TEST--
Io\Poll\Context::wait() returns early for a signal PHP has a handler for
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (!function_exists('pcntl_alarm')) die('skip pcntl_alarm() not available');
?>
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

[$r, $w] = pt_new_socket_pair();
$ctx = pt_new_stream_poll();
pt_stream_poll_add($ctx, $r, [Io\Poll\Event::Read], 'r');

/* Without a timeout, only the signal ends these waits */
pcntl_async_signals(true);
pcntl_signal(SIGALRM, function () { echo "handler\n"; });
pcntl_alarm(1);
var_dump($ctx->wait());

pcntl_async_signals(false);
pcntl_alarm(1);
var_dump($ctx->wait());
echo "dispatch\n";
pcntl_signal_dispatch();

fwrite($w, "x");
var_dump(count($ctx->wait()));
?>
--EXPECT--
handler
array(0) {
}
array(0) {
}
dispatch
handler
int(1)
