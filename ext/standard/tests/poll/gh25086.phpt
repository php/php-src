--TEST--
GH-25086 (Io\Poll kqueue backend leaks memory on every Watcher::modifyEvents() call)
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

[$r, $w] = pt_new_socket_pair();
$context = pt_new_stream_poll();
$watcher = pt_stream_poll_add($context, $r, [Io\Poll\Event::Read]);

$modify = function (int $count) use ($context, $watcher) {
    for ($i = 0; $i < $count; $i++) {
        $watcher->modifyEvents($i % 2 ? [Io\Poll\Event::Read] : [Io\Poll\Event::Read, Io\Poll\Event::Write]);
        $context->wait(Time\Duration::fromSeconds(0));
    }
};

$modify(10);
$before = memory_get_usage();
$modify(1000);
var_dump(memory_get_usage() - $before);
?>
--EXPECT--
int(0)
