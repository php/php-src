--TEST--
Io\Poll\ProcessHandle on Windows: a process object that answers the exit code, no source to poll
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip Windows only');
?>
--FILE--
<?php
$proc = proc_open([PHP_BINARY, '-n', '-r', 'usleep(200000); exit(5);'], [], $pipes);
$handle = Io\Poll\ProcessHandle::fromProcess($proc);
var_dump($handle->getPid() === proc_get_status($proc)['pid'], $handle->getStatus());

// No source for the poll queue: the context refuses it
$ctx = new Io\Poll\Context();
try {
    $ctx->add($handle, [Io\Poll\Event::Process]);
} catch (Io\Poll\FailedHandleAddException $e) {
    echo get_class($e), ": ", $e->getMessage(), "\n";
}

// The exit code once the process ended, whoever waited for it
var_dump(proc_close($proc));
var_dump($handle->getStatus());

// A pid nobody runs under
try {
    new Io\Poll\ProcessHandle(0x7FFFFFFE);
} catch (ValueError $e) {
    echo get_class($e), ": ", substr($e->getMessage(), 0, 72), "\n";
}
?>
--EXPECTF--
bool(true)
NULL
Io\Poll\FailedHandleAddException: This backend has no source for the handle
int(5)
int(5)
ValueError: Io\Poll\ProcessHandle::__construct(): Argument #1 ($pid) must be t%s
