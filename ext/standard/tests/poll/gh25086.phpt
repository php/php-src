--TEST--
GH-25086 (Io\Poll kqueue backend leaks memory on every Watcher::modifyEvents() call)
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

function test_modify(array $events_list): void {
    [$r, $w] = pt_new_socket_pair();
    fwrite($w, "x");
    $context = pt_new_stream_poll();
    $watcher = pt_stream_poll_add($context, $r, $events_list[0]);

    $modify = function (int $count) use ($context, $watcher, $events_list) {
        for ($i = 0; $i < $count; $i++) {
            $context->wait(Time\Duration::fromSeconds(0));
            $watcher->modifyEvents($events_list[$i % count($events_list)]);
        }
    };

    $modify(10);
    $before = memory_get_usage();
    $modify(1000);
    var_dump(memory_get_usage() - $before);
}

test_modify([[Io\Poll\Event::Read], [Io\Poll\Event::Read, Io\Poll\Event::Write]]);
test_modify([[Io\Poll\Event::Read, Io\Poll\Event::OneShot]]);
test_modify([[Io\Poll\Event::Read, Io\Poll\Event::Write, Io\Poll\Event::OneShot]]);
?>
--EXPECT--
int(0)
int(0)
int(0)
