--TEST--
IO hooks: proc_open() pipes are read and written through the provider, so a waiting read lets the other fibers run
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
// Installed before proc_open(): on Windows the pipes are made overlapped for the provider only then
Io\Hooks\set_hooks($scheduler);

$ticks = 0;
$ticks_at_read = -1;
$scheduler->spawn(function () use ($php, &$ticks, &$ticks_at_read) {
    // The child answers after a while: the read waits on the provider, not on the thread
    $proc = proc_open([$php, '-n', '-r', 'usleep(200000); echo "late";'],
        [1 => ['pipe', 'w']], $pipes);
    var_dump(fread($pipes[1], 100));
    $ticks_at_read = $ticks;
    var_dump(fread($pipes[1], 100), feof($pipes[1]));
    fclose($pipes[1]);
    var_dump(proc_close($proc));
});
$scheduler->spawn(function () use (&$ticks, &$ticks_at_read) {
    // Runs while the read waits
    while ($ticks_at_read < 0 && $ticks < 1000) {
        usleep(5000);
        $ticks++;
    }
});
$scheduler->loop();
var_dump($ticks_at_read > 0);

// A write to the child's input, through the provider too, and the answer back
$scheduler->spawn(function () use ($php) {
    $proc = proc_open([$php, '-n', '-r', 'echo strtoupper(fgets(STDIN));'],
        [0 => ['pipe', 'r'], 1 => ['pipe', 'w']], $pipes);
    var_dump(fwrite($pipes[0], "hello\n"));
    fclose($pipes[0]);
    var_dump(stream_get_contents($pipes[1]));
    fclose($pipes[1]);
    var_dump(proc_close($proc));
});
$scheduler->loop();

// A larger body than one pipe buffer, both ways
$scheduler->spawn(function () use ($php, $scheduler) {
    $proc = proc_open([$php, '-n', '-r', 'echo stream_get_contents(STDIN);'],
        [0 => ['pipe', 'r'], 1 => ['pipe', 'w']], $pipes);
    $body = str_repeat("0123456789abcdef", 20000);
    // The write waits for the child to read: on a fiber of its own, while this one reads
    $scheduler->spawn(function () use ($pipes, $body) {
        var_dump(fwrite($pipes[0], $body));
        fclose($pipes[0]);
    });
    var_dump(stream_get_contents($pipes[1]) === $body);
    fclose($pipes[1]);
    var_dump(proc_close($proc));
});
$scheduler->loop();
Io\Hooks\set_hooks(null);
?>
--EXPECT--
string(4) "late"
string(0) ""
bool(true)
int(0)
bool(true)
int(6)
string(6) "HELLO
"
int(0)
int(320000)
bool(true)
int(0)
