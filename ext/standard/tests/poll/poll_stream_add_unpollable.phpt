--TEST--
Io\Poll: epoll reports a handle it cannot watch as unsupported
--SKIPIF--
<?php
require_once __DIR__ . '/poll.inc';
if ('Epoll' !== pt_new_stream_poll()->getBackend()->name) {
    die("skip requires the epoll backend\n");
}
?>
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

$ctx = pt_new_stream_poll();

// epoll_ctl() rejects files that do not implement polling
foreach (['regular file' => fopen(__FILE__, 'r'), '/dev/null' => fopen('/dev/null', 'r')] as $label => $stream) {
    try {
        $ctx->add(new StreamPollHandle($stream), [Io\Poll\Event::Read]);
        echo "$label: added\n";
    } catch (Io\Poll\FailedHandleAddException $e) {
        printf("%s: %s, code %s\n", $label, $e->getMessage(),
            Io\Poll\FailedPollOperationException::ERROR_NOSUPPORT === $e->getCode() ? 'ERROR_NOSUPPORT' : $e->getCode());
    }
    fclose($stream);
}

// a pollable handle still works
[$r, $w] = pt_new_socket_pair();
$ctx->add(new StreamPollHandle($r), [Io\Poll\Event::Read]);
echo "socket: added\n";
?>
--EXPECT--
regular file: Failed to add handle, code ERROR_NOSUPPORT
/dev/null: Failed to add handle, code ERROR_NOSUPPORT
socket: added
