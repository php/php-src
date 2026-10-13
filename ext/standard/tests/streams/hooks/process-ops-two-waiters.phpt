--TEST--
IO hooks: two waits for one child, the one that collects it gets the status and the other ECHILD
--EXTENSIONS--
pcntl
posix
--FILE--
<?php
include __DIR__ . '/scheduler.inc';

$scheduler = new Scheduler();
Io\Hooks\set_hooks($scheduler);

$pid = pcntl_fork();
if ($pid === 0) {
    Io\Hooks\set_hooks(null);
    usleep(100000);
    exit(3);
}

$results = [];
foreach (['a', 'b'] as $name) {
    $scheduler->spawn(function () use ($pid, $name, &$results) {
        $r = pcntl_waitpid($pid, $status);
        $results[$name] = $r === $pid ? pcntl_wexitstatus($status) : [$r, pcntl_get_last_error() === PCNTL_ECHILD];
    });
}
$scheduler->loop();
Io\Hooks\set_hooks(null);

sort($results);
var_dump($results);
?>
--EXPECT--
array(2) {
  [0]=>
  int(3)
  [1]=>
  array(2) {
    [0]=>
    int(-1)
    [1]=>
    bool(true)
  }
}
