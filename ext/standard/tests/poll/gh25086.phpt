--TEST--
GH-25086 (Io\Poll kqueue backend leaks memory on every Watcher::modifyEvents() call)
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

function test_modify(string $name, array $events_list, bool $readable): void {
    [$r, $w] = pt_new_socket_pair();
    if ($readable) {
        fwrite($w, "x");
    }
    $context = pt_new_stream_poll();
    $watcher = pt_stream_poll_add($context, $r, $events_list[0]);

    $modify = function (int $count) use ($context, $watcher, $events_list) {
        for ($i = 0; $i < $count; $i++) {
            $context->wait(Time\Duration::fromSeconds(0));
            $watcher->modifyEvents($events_list[$i % count($events_list)]);
        }
    };

    // Warm up to absorb one-time allocations
    $modify(10);
    $before = memory_get_usage();
    $modify(1000);
    $growth = memory_get_usage() - $before;
    echo "$name: ", $growth < 1024 ? "no growth" : "grew by $growth bytes", "\n";
    $watcher->remove();
}

use Io\Poll\Event;

test_modify("read <-> read+write", [[Event::Read], [Event::Read, Event::Write]], true);
test_modify("read <-> write", [[Event::Read], [Event::Write]], true);
test_modify("read -> read one-shot -> read", [[Event::Read], [Event::Read, Event::OneShot]], true);
test_modify("re-arm read one-shot", [[Event::Read, Event::OneShot]], true);
test_modify("re-arm read+write one-shot, both fire", [[Event::Read, Event::Write, Event::OneShot]], true);
test_modify("re-arm read+write one-shot, write fires", [[Event::Read, Event::Write, Event::OneShot]], false);
?>
--EXPECT--
read <-> read+write: no growth
read <-> write: no growth
read -> read one-shot -> read: no growth
re-arm read one-shot: no growth
re-arm read+write one-shot, both fire: no growth
re-arm read+write one-shot, write fires: no growth
