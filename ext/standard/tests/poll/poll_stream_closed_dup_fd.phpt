--TEST--
Io\Poll: a watched stream closed while a duplicated fd exists is unregistered in time
--SKIPIF--
<?php
if (!function_exists('proc_open')) {
    die("skip proc_open required\n");
}
if (!Io\Poll\Backend::Epoll->isAvailable()) {
    die("skip Epoll backend required\n");
}
?>
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

// The child keeps a duplicate of $r open, so epoll would keep the interest after
// fclose() unless the watcher is unregistered while the fd is still open
$ctx = new Io\Poll\Context(Io\Poll\Backend::Epoll);
list($r, $w) = pt_new_socket_pair();
$watcher = $ctx->add(new StreamPollHandle($r), [Io\Poll\Event::Read]);

$proc = proc_open('sleep 2', [0 => $r], $pipes);
fclose($r);
$watcher->remove();
unset($watcher);
gc_collect_cycles();

fwrite($w, "ping");
echo "Events count: ", count($ctx->wait(Time\Duration::fromSeconds(0))), "\n";

// A new stream that takes the same fd number gets its own registration
list($r2, $w2) = pt_new_socket_pair();
$watcher2 = $ctx->add(new StreamPollHandle($r2), [Io\Poll\Event::Read], "new");
echo "Events count: ", count($ctx->wait(Time\Duration::fromSeconds(0))), "\n";
fwrite($w2, "pong");
$events = $ctx->wait(Time\Duration::fromMicroseconds(100000));
echo "Events count: ", count($events), "\n";
var_dump($events[0]->getData());

proc_terminate($proc);
proc_close($proc);
echo "done\n";
?>
--EXPECT--
Events count: 0
Events count: 0
Events count: 1
string(3) "new"
done
