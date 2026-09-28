--TEST--
SocketPollHandle and SocketPollWeakHandle: polling a Socket, retired when it closes
--EXTENSIONS--
sockets
--FILE--
<?php
socket_create_pair(AF_UNIX, SOCK_STREAM, 0, $pair);
[$a, $b] = $pair;

$h = new SocketPollHandle($a);
var_dump($h->getSocket() === $a, $h->isValid());

$ctx = new Io\Poll\Context();
$ctx->onWatcherRemoved(function ($w) { echo "removed: ", get_class($w->getHandle()), "\n"; });
$w = $ctx->add($h, [Io\Poll\Event::Read]);
socket_write($b, "x");
$fired = $ctx->wait(Time\Duration::fromSeconds(1));
var_dump(count($fired), $fired[0] === $w, $fired[0]->hasTriggered(Io\Poll\Event::Read));

// Closing the Socket retires the watcher while the descriptor is open; the handle keeps the Socket
socket_close($a);
var_dump($w->isActive(), $h->isValid(), $h->getSocket() === $a);
var_dump($ctx->wait(Time\Duration::fromSeconds(0)));

// The weak handle is the Socket's one object and reports the close
$wk = SocketPollWeakHandle::create($b);
var_dump($wk === SocketPollWeakHandle::create($b), $wk->getSocket() === $b, $wk->isValid());
$w2 = $ctx->add($wk, [Io\Poll\Event::Read]);
socket_close($b);
var_dump($w2->isActive(), $wk->isValid(), $wk->getSocket());
var_dump($ctx->wait(Time\Duration::fromSeconds(0)));

try {
    new SocketPollWeakHandle();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    clone $wk;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    new SocketPollHandle($a);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    SocketPollWeakHandle::create($b);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
bool(true)
int(1)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
removed: SocketPollHandle
array(0) {
}
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
NULL
removed: SocketPollWeakHandle
array(0) {
}
Call to private SocketPollWeakHandle::__construct() from global scope
Trying to clone an uncloneable object of class SocketPollWeakHandle
SocketPollHandle::__construct(): Argument #1 ($socket) has already been closed
SocketPollWeakHandle::create(): Argument #1 ($socket) has already been closed
