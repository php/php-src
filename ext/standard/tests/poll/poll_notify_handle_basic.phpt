--TEST--
Io\Poll\NotifyHandle: raised by notify(), ready until clear()
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

$ctx = pt_new_stream_poll();
$notify = new Io\Poll\NotifyHandle();
var_dump($notify instanceof Io\Poll\Handle);

$watcher = $ctx->add($notify, [Io\Poll\Event::Notify], "notify");
var_dump($watcher->getWatchedEvents());
echo "Events count: ", count($ctx->wait(Time\Duration::fromSeconds(0))), "\n";

$notify->notify();
$notify->notify();
$events = $ctx->wait(Time\Duration::fromMilliseconds(100));
echo "Events count: ", count($events), "\n";
var_dump($events[0] === $watcher, $events[0]->getTriggeredEvents(), $events[0]->hasTriggered(Io\Poll\Event::Notify));

// Level: still reported until cleared
echo "Events count: ", count($ctx->wait(Time\Duration::fromSeconds(0))), "\n";
$notify->clear();
echo "Events count: ", count($ctx->wait(Time\Duration::fromSeconds(0))), "\n";

// Raised again after clearing
$notify->notify();
echo "Events count: ", count($ctx->wait(Time\Duration::fromMilliseconds(100))), "\n";
$notify->clear();

try {
    $ctx->add(new Io\Poll\NotifyHandle(), [Io\Poll\Event::Read]);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $ctx->add(new Io\Poll\NotifyHandle(), [Io\Poll\Event::Notify, Io\Poll\Event::Write]);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    [$r, $w] = pt_new_socket_pair();
    $ctx->add(new StreamPollHandle($r), [Io\Poll\Event::Notify]);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

$watcher->remove();
var_dump($watcher->isActive());
?>
--EXPECT--
bool(true)
array(1) {
  [0]=>
  enum(Io\Poll\Event::Notify)
}
Events count: 0
Events count: 1
bool(true)
array(1) {
  [0]=>
  enum(Io\Poll\Event::Notify)
}
bool(true)
Events count: 1
Events count: 0
Events count: 1
Io\Poll\Context::add(): Argument #2 ($events) must be Event::Notify for a NotifyHandle
Io\Poll\Context::add(): Argument #2 ($events) must be Event::Notify for a NotifyHandle
Io\Poll\Context::add(): Argument #2 ($events) must not contain Event::Timer, Event::Notify, Event::Signal or Event::Process for this handle
bool(false)
