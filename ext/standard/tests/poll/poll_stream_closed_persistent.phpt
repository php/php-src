--TEST--
Io\Poll: a watcher on a persistent stream is unregistered at request shutdown
--SKIPIF--
<?php
$srv = @stream_socket_server("tcp://127.0.0.1:0", $e1, $e2);
if (!$srv) {
    die("skip cannot bind loopback listener\n");
}
?>
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

$srv = stream_socket_server("tcp://127.0.0.1:0", $e1, $e2);
$addr = stream_socket_get_name($srv, false);
$p = pfsockopen("tcp://" . $addr, -1, $en, $es, 1);

$ctx = pt_new_stream_poll();
$watcher = $ctx->add(new StreamPollHandle($p), [Io\Poll\Event::Write]);
echo "Events count: ", count($ctx->wait(Time\Duration::fromMicroseconds(100000))), "\n";

// The persistent stream outlives the request, so the watcher and context are
// intentionally left to be freed at shutdown
echo "done\n";
?>
--EXPECT--
Events count: 1
done
