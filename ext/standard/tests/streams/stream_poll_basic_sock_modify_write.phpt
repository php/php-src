--TEST--
Stream polling basic functionality
--FILE--
<?php
require_once __DIR__ . '/stream_poll.inc';

$sockets = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
if ($sockets === false) {
    echo "Cannot create socket pair\n";
    exit(0);
}
list($socket1, $socket2) = $sockets;

$poll_ctx = new_stream_poll();

stream_poll_add($poll_ctx, $socket2, STREAM_POLL_WRITE, "socket_data");
stream_poll_modify($poll_ctx, $socket2, STREAM_POLL_WRITE, "modified_data");

$events = stream_poll_wait($poll_ctx, 0);
var_dump(is_array($events));
echo "Events count: " . count($events) . "\n";

foreach ($events as $event) {
    echo "Event: ";
    var_dump($event instanceof StreamPollEvent);
    if ($event instanceof StreamPollEvent) {
        echo "Events: " . $event->events . ", Data: " . $event->data . "\n";
    }
}

// Clean up
fclose($socket1);
fclose($socket2);

?>
--EXPECT--
bool(true)
Events count: 1
Event: bool(true)
Events: 2, Data: modified_data
