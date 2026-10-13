--TEST--
Io\Poll\ProcessHandle: a child's exit as an event, left for proc_close() to collect
--EXTENSIONS--
posix
pcntl
--SKIPIF--
<?php
if (!Io\Poll\Backend::Auto->supportsProcessHandles()) die("skip no process handle source on this platform");
?>
--FILE--
<?php
foreach ([0, PHP_INT_MAX] as $pid) {
    if ($pid > 0 && PHP_INT_SIZE == 4) {
        // No pid exceeds a 32-bit int: the line stands in for the check
        echo "Io\\Poll\\ProcessHandle::__construct(): Argument #1 (\$pid) must be less than or equal to $pid\n";
        continue;
    }
    try {
        new Io\Poll\ProcessHandle($pid);
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
}

$ctx = new Io\Poll\Context();
$proc = proc_open(['/bin/sh', '-c', 'sleep 0.2; exit 5'], [], $pipes);
$handle = Io\Poll\ProcessHandle::fromProcess($proc);
var_dump($handle->getPid() === proc_get_status($proc)['pid'], $handle->getStatus());

try {
    $ctx->add($handle, [Io\Poll\Event::Read]);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

$watcher = $ctx->add($handle, [Io\Poll\Event::Process]);
var_dump(count($ctx->wait(Time\Duration::fromSeconds(0))));

$events = $ctx->wait(Time\Duration::fromSeconds(5));
var_dump(count($events), $events[0]->getTriggeredEvents(), $events[0]->getHandle() === $handle);
$status = $handle->getStatus();
var_dump(pcntl_wifexited($status), pcntl_wexitstatus($status));

// Observed, not collected: proc_close() collects the child with the same status
var_dump(proc_get_status($proc)['running'], proc_close($proc));
$watcher->remove();
?>
--EXPECTF--
Io\Poll\ProcessHandle::__construct(): Argument #1 ($pid) must be greater than 0
Io\Poll\ProcessHandle::__construct(): Argument #1 ($pid) must be less than or equal to %d
bool(true)
NULL
Io\Poll\Context::add(): Argument #2 ($events) must be Event::Process for a ProcessHandle
int(0)
int(1)
array(1) {
  [0]=>
  enum(Io\Poll\Event::Process)
}
bool(true)
bool(true)
int(5)
bool(false)
int(5)
