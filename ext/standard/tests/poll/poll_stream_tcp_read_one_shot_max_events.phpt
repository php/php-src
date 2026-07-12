--TEST--
Poll stream - TCP read oneshot with maxEvents smaller than the number of ready FDs
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

list($clients, $servers) = pt_new_tcp_socket_connections(8);
$poll_ctx = pt_new_stream_poll();

for ($i = 0; $i < count($servers); $i++) {
    pt_stream_poll_add($poll_ctx, $servers[$i], [Io\Poll\Event::Read, Io\Poll\Event::OneShot], "server$i");
}

for ($i = 0; $i < count($clients); $i++) {
    pt_write_sleep($clients[$i], "test $i data");
}

// 8 oneshot FDs are ready but only 4 events fit into the buffer. The events
// that did not fit must not be consumed - the following wait() has to deliver
// them, and each oneshot event must be delivered exactly once.
$seen = [];
for ($round = 1; $round <= 2; $round++) {
    $watchers = $poll_ctx->wait(0, 100000, 4);
    echo "Round $round count: " . count($watchers) . "\n";
    foreach ($watchers as $watcher) {
        $data = $watcher->getData();
        if (isset($seen[$data])) {
            echo "Duplicate event for $data\n";
        }
        $seen[$data] = true;
    }
}

ksort($seen);
echo "Seen: " . implode(',', array_keys($seen)) . "\n";

// Every oneshot watcher fired once, so new data must not be reported
pt_write_sleep($clients[0], "more data");
pt_expect_events($poll_ctx->wait(0, 100000), []);

?>
--EXPECT--
Round 1 count: 4
Round 2 count: 4
Seen: server0,server1,server2,server3,server4,server5,server6,server7
Events matched - count: 0
