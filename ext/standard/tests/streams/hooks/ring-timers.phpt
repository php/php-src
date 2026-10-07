--TEST--
Io\Ring\Engine: Timer ops fire from the wrapper's deadline heap, in order, and leave nothing behind
--SKIPIF--
<?php if (!class_exists(Io\Ring\Engine::class)) die("skip Io\\Ring\\Engine not available"); ?>
--FILE--
<?php
include __DIR__ . '/scheduler.inc';

$ring = new Io\Ring\Engine();
$scheduler = new Scheduler($ring);
Io\Hooks\set_hooks($scheduler);

// Sleeps complete in the order of their deadlines, not of their submission
$order = [];
foreach ([30, 10, 20, 0] as $ms) {
    $scheduler->spawn(function () use ($ms, &$order) {
        usleep($ms * 1000);
        $order[] = $ms;
    });
}
$start = hrtime(true);
$scheduler->loop();
var_dump($order, (hrtime(true) - $start) / 1e6 >= 29);

// A select answered before its timeout drops its Timer member from the heap
[$r, $w] = stream_socket_pair(PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
$scheduler->spawn(function () use ($r) {
    $read = [$r];
    $write = null;
    $except = null;
    var_dump(stream_select($read, $write, $except, 5));
});
$scheduler->spawn(function () use ($w) {
    usleep(10000);
    fwrite($w, "x");
});
$scheduler->loop();
var_dump($ring->countPending());

// A thousand sleeps with one deadline wake in one wait
$n = 1000;
$woken = 0;
$deadline = hrtime(true) + 50_000_000;
for ($i = 0; $i < $n; $i++) {
    $scheduler->spawn(function () use ($deadline, &$woken) {
        usleep(max(1, intdiv($deadline - hrtime(true), 1000)));
        $woken++;
    });
}
$scheduler->loop();
var_dump($woken, $ring->countPending());
Io\Hooks\set_hooks(null);
?>
--EXPECT--
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(10)
  [2]=>
  int(20)
  [3]=>
  int(30)
}
bool(true)
int(1)
int(0)
int(1000)
int(0)
