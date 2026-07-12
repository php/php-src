--TEST--
Poll stream - TCP read with maxEvents smaller than the number of ready FDs
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

list($clients, $servers) = pt_new_tcp_socket_connections(8);
$poll_ctx = pt_new_stream_poll();

for ($i = 0; $i < count($servers); $i++) {
    pt_stream_poll_add($poll_ctx, $servers[$i], [Io\Poll\Event::Read], "server$i");
}

for ($i = 0; $i < count($clients); $i++) {
    pt_write_sleep($clients[$i], "test $i data");
}

// 8 FDs are ready but only 4 events fit into the buffer, so the remaining
// events must be delivered by the following wait() without any duplicates.
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
        // Drain so level triggering does not re-report the FD
        fread($watcher->getHandle()->getStream(), 1024);
    }
}

ksort($seen);
echo "Seen: " . implode(',', array_keys($seen)) . "\n";

pt_expect_events($poll_ctx->wait(0), []);

?>
--EXPECT--
Round 1 count: 4
Round 2 count: 4
Seen: server0,server1,server2,server3,server4,server5,server6,server7
Events matched - count: 0
