--TEST--
Io\Poll: a timeout that overflows an int of milliseconds keeps waiting
--SKIPIF--
<?php
if (!Io\Poll\Backend::Poll->isAvailable()) {
    die("skip poll backend not available\n");
}
if (PHP_OS_FAMILY === 'Windows') {
    die("skip POSIX only\n");
}
?>
--FILE--
<?php
$process = proc_open([PHP_BINARY, '-r', 'usleep(300000); echo "ready";'], [1 => ['pipe', 'w']], $pipes);

$poll_ctx = new Io\Poll\Context(Io\Poll\Backend::Poll);
$watcher = $poll_ctx->add(new StreamPollHandle($pipes[1]), [Io\Poll\Event::Read]);

// 158913790 seconds is about 5 years, and wraps around to 48ms as an int of milliseconds
$events = $poll_ctx->wait(Time\Duration::fromSeconds(158913790));

var_dump(count($events), $events[0] === $watcher);

fclose($pipes[1]);
proc_close($process);
?>
--EXPECT--
int(1)
bool(true)
