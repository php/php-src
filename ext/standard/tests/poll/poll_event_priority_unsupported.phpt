--TEST--
Io\Poll: Event::Priority is refused with ERROR_NOSUPPORT where the backend has no source for it
--SKIPIF--
<?php
require_once __DIR__ . '/poll.inc';
if (pt_new_stream_poll()->getBackend()->supportsPriority()) die("skip Priority is supported by this backend");
?>
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

$ctx = pt_new_stream_poll();
[$client, $server] = pt_new_tcp_socket_pair();

try {
    $ctx->add(new StreamPollHandle($server), [Io\Poll\Event::Priority]);
} catch (Io\Poll\FailedHandleAddException $e) {
    var_dump($e->getCode() === Io\Poll\FailedHandleAddException::ERROR_NOSUPPORT);
}

$watcher = $ctx->add(new StreamPollHandle($server), [Io\Poll\Event::Read]);
try {
    $watcher->modifyEvents([Io\Poll\Event::Read, Io\Poll\Event::Priority]);
} catch (Io\Poll\FailedWatcherModificationException $e) {
    var_dump($e->getCode() === Io\Poll\FailedWatcherModificationException::ERROR_NOSUPPORT);
}
$watcher->remove();
echo "done\n";
?>
--EXPECT--
bool(true)
bool(true)
done
