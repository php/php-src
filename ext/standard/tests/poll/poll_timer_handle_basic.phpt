--TEST--
Io\Poll\TimerHandle: one-shot and periodic timers on the context's deadline heap
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

$ctx = pt_new_stream_poll();

$timer = new Io\Poll\TimerHandle(Time\Duration::fromMilliseconds(30));
var_dump($timer instanceof Io\Poll\Handle, $timer->isPeriodic(), Time\Duration::compare($timer->getTimeout(), Time\Duration::fromMilliseconds(30)));

$watcher = $ctx->add($timer, [Io\Poll\Event::Timer], "one-shot");
var_dump($watcher->getWatchedEvents());

// Not yet
echo "Events count: ", count($ctx->wait(Time\Duration::fromSeconds(0))), "\n";

// The timer bounds an unbounded wait
$start = hrtime(true);
$events = $ctx->wait();
$elapsed_ms = (hrtime(true) - $start) / 1e6;
echo "Events count: ", count($events), "\n";
var_dump($events[0] === $watcher, $events[0]->getData(), $events[0]->getTriggeredEvents());
var_dump($elapsed_ms >= 25 && $elapsed_ms < 1000);

// Fired once: stays active, disarmed
var_dump($watcher->isActive());
echo "Events count: ", count($ctx->wait(Time\Duration::fromMilliseconds(60))), "\n";

// Re-armed by modifyEvents()
$watcher->modifyEvents([Io\Poll\Event::Timer]);
echo "Events count: ", count($ctx->wait(Time\Duration::fromSeconds(0))), "\n";
echo "Events count: ", count($ctx->wait(Time\Duration::fromMilliseconds(500))), "\n";

$watcher->remove();
var_dump($watcher->isActive());
try {
    $watcher->remove();
} catch (Io\Poll\InactiveWatcherException $e) {
    echo $e->getMessage(), "\n";
}

// Periodic
$periodic = new Io\Poll\TimerHandle(Time\Duration::fromMilliseconds(10), true);
var_dump($periodic->isPeriodic());
$pw = $ctx->add($periodic, [Io\Poll\Event::Timer], "periodic");
$fired = 0;
$start = hrtime(true);
while ($fired < 3) {
    foreach ($ctx->wait(Time\Duration::fromSeconds(2)) as $ev) {
        var_dump($ev === $pw);
        $fired++;
    }
}
$elapsed_ms = (hrtime(true) - $start) / 1e6;
var_dump($elapsed_ms >= 25 && $elapsed_ms < 2000);
$pw->remove();

// Wrong events for a timer
try {
    $ctx->add(new Io\Poll\TimerHandle(Time\Duration::fromSeconds(1)), [Io\Poll\Event::Read]);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    new Io\Poll\TimerHandle(Time\Duration::fromSeconds(0), true);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
bool(false)
int(0)
array(1) {
  [0]=>
  enum(Io\Poll\Event::Timer)
}
Events count: 0
Events count: 1
bool(true)
string(8) "one-shot"
array(1) {
  [0]=>
  enum(Io\Poll\Event::Timer)
}
bool(true)
bool(true)
Events count: 0
Events count: 0
Events count: 1
bool(false)
Cannot remove inactive watcher
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
Io\Poll\Context::add(): Argument #2 ($events) must be Event::Timer for a TimerHandle
Io\Poll\TimerHandle::__construct(): Argument #1 ($timeout) must not be zero for a periodic timer
