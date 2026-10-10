--TEST--
IO hooks: waitCompletions() returns early for a signal, so its handler runs while ops are pending
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (!function_exists('pcntl_alarm')) die('skip pcntl_alarm() not available');
?>
--FILE--
<?php
include __DIR__ . '/scheduler.inc';

$queue = Scheduler::defaultQueue();
[$a, $b] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
/* The handler runs on the main flow, which cannot suspend in the provider */
stream_set_blocking($b, false);

pcntl_async_signals(true);
pcntl_signal(SIGALRM, function () use ($b) {
    echo "handler\n";
    fwrite($b, "x");
});

/* Only the handler can complete the read */
$scheduler = new Scheduler($queue);
Io\Hooks\set_hooks($scheduler);
$scheduler->spawn(function () use ($a) {
    var_dump(fread($a, 10));
});
pcntl_alarm(1);
$scheduler->loop();

/* The dispatch style: an empty array first */
pcntl_async_signals(false);
$fiber = new Fiber(function () use ($a) {
    var_dump(fread($a, 10));
});
$fiber->start();
pcntl_alarm(1);
var_dump($queue->waitCompletions());
pcntl_signal_dispatch();
$completions = $queue->waitCompletions();
var_dump(count($completions));
$fiber->resume($completions[0]);
Io\Hooks\set_hooks(null);
?>
--EXPECT--
handler
string(1) "x"
array(0) {
}
handler
int(1)
string(1) "x"
