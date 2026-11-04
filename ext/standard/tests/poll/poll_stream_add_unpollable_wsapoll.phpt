--TEST--
Io\Poll: WSAPoll reports a handle that is not a socket as unsupported
--SKIPIF--
<?php
if (!Io\Poll\Backend::WSAPoll->isAvailable()) {
    die("skip WSAPoll backend required\n");
}
if (!function_exists('proc_open')) {
    die("skip proc_open required\n");
}
?>
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

$ctx = new Io\Poll\Context(Io\Poll\Backend::WSAPoll);

// WSAPoll() only polls sockets, while files and pipes give a CRT fd
$proc = proc_open([PHP_BINARY, '-n', '-r', 'echo "done";'], [1 => ['pipe', 'w']], $pipes);
foreach (['regular file' => fopen(__FILE__, 'r'), 'pipe' => $pipes[1]] as $label => $stream) {
    try {
        $ctx->add(new StreamPollHandle($stream), [Io\Poll\Event::Read]);
        echo "$label: added\n";
    } catch (Io\Poll\FailedHandleAddException $e) {
        printf("%s: %s, code %s\n", $label, $e->getMessage(),
            Io\Poll\FailedPollOperationException::ERROR_NOSUPPORT === $e->getCode() ? 'ERROR_NOSUPPORT' : $e->getCode());
    }
    fclose($stream);
}
proc_close($proc);

// a socket is still added and reported
[$r, $w] = pt_new_socket_pair();
$watcher = $ctx->add(new StreamPollHandle($r), [Io\Poll\Event::Read]);
echo "socket: added\n";
fwrite($w, "ping");
$events = $ctx->wait(Time\Duration::fromSeconds(5));
var_dump(count($events), $events[0] === $watcher);
?>
--EXPECT--
regular file: Failed to add handle, code ERROR_NOSUPPORT
pipe: Failed to add handle, code ERROR_NOSUPPORT
socket: added
int(1)
bool(true)
