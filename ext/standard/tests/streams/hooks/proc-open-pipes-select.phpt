--TEST--
IO hooks: stream_select() and a non-blocking read on proc_open() pipes, and a pipe handed to a second child
--SKIPIF--
<?php
include __DIR__ . '/scheduler.inc';
if (PHP_OS_FAMILY === 'Windows' && !(Scheduler::defaultQueue() instanceof Io\Ring\Engine)) {
    die("skip on Windows only a ring performs a pipe op: a readiness queue leaves the wait to the thread");
}
?>
--FILE--
<?php
include __DIR__ . '/scheduler.inc';

$php = PHP_BINARY;
$scheduler = new Scheduler();
Io\Hooks\set_hooks($scheduler);

$scheduler->spawn(function () use ($php) {
    $proc = proc_open([$php, '-n', '-r', 'usleep(150000); echo "one"; usleep(150000); echo "two";'],
        [1 => ['pipe', 'w']], $pipes);
    $w = $e = null;

    // Nothing there yet: a non-blocking read returns nothing, and a short select times out
    stream_set_blocking($pipes[1], false);
    var_dump(fread($pipes[1], 10));
    $r = [$pipes[1]];
    var_dump(stream_select($r, $w, $e, 0, 20000), count($r));

    // The select waits for the bytes
    $r = [$pipes[1]];
    var_dump(stream_select($r, $w, $e, 5), count($r));
    var_dump(fread($pipes[1], 10));

    // A blocking read waits for them
    stream_set_blocking($pipes[1], true);
    var_dump(fread($pipes[1], 10));

    // The writer's close is reported, and the read sees the end
    $r = [$pipes[1]];
    var_dump(stream_select($r, $w, $e, 5), fread($pipes[1], 10), feof($pipes[1]));
    fclose($pipes[1]);
    var_dump(proc_close($proc));
});
$scheduler->loop();

// The first child's output pipe is the second child's input: the parent reads the second's output
$scheduler->spawn(function () use ($php) {
    $first = proc_open([$php, '-n', '-r', 'usleep(100000); echo "handed";'], [1 => ['pipe', 'w']], $fp);
    $second = proc_open([$php, '-n', '-r', 'echo strtoupper(stream_get_contents(STDIN));'],
        [0 => $fp[1], 1 => ['pipe', 'w']], $sp);
    fclose($fp[1]);
    var_dump(stream_get_contents($sp[1]));
    fclose($sp[1]);
    var_dump(proc_close($second), proc_close($first));
});
$scheduler->loop();
Io\Hooks\set_hooks(null);
?>
--EXPECT--
string(0) ""
int(0)
int(0)
int(1)
int(1)
string(3) "one"
string(3) "two"
int(1)
string(0) ""
bool(true)
int(0)
string(6) "HANDED"
int(0)
int(0)
