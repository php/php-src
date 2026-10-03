--TEST--
Io\Poll\ProcessHandle: a child that exited before it was watched is reported at once
--EXTENSIONS--
posix
pcntl
--SKIPIF--
<?php
if (!Io\Poll\Backend::Auto->supportsProcessHandles()) die("skip no process handle source on this platform");
?>
--FILE--
<?php
$ctx = new Io\Poll\Context();
$proc = proc_open(['/bin/sh', '-c', 'exit 3'], [], $pipes);
// Let it exit before the handle exists; it stays a zombie until collected
usleep(200000);
$handle = Io\Poll\ProcessHandle::fromProcess($proc);
var_dump($handle->getStatus());

$watcher = $ctx->add($handle, [Io\Poll\Event::Process]);
$events = $ctx->wait(Time\Duration::fromSeconds(5));
var_dump(count($events), $events[0]->getTriggeredEvents());
$status = $handle->getStatus();
var_dump(pcntl_wifexited($status), pcntl_wexitstatus($status));

// Reported once, then no more
var_dump(count($ctx->wait(Time\Duration::fromSeconds(0))));
var_dump(proc_close($proc));
$watcher->remove();

// Reaped already: not a process to watch
proc_close(proc_open(['/bin/sh', '-c', 'exit 0'], [], $pipes));
try {
    $handle = new Io\Poll\ProcessHandle(999999);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
NULL
int(1)
array(1) {
  [0]=>
  enum(Io\Poll\Event::Process)
}
bool(true)
int(3)
int(0)
int(3)
Io\Poll\ProcessHandle::__construct(): Argument #1 ($pid) must be the id of a running process: %s
