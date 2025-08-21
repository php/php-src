--TEST--
Stream polling basic functionality - socket write / read
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

stream_poll_add($poll_ctx, $socket1, STREAM_POLL_READ, "socket1_data");
stream_poll_add($poll_ctx, $socket2, STREAM_POLL_WRITE, "socket2_data");

$events = stream_poll_wait($poll_ctx, 0);
var_dump(is_array($events));
echo "Events count: " . count($events) . "\n";

foreach ($events as $event) {
    echo "Event: ";
    if ($event instanceof StreamPollEvent) {
        echo "Events: " . $event->events . ", Data: " . $event->data . "\n";
    }
}

fwrite($socket2, "test data");
$events = stream_poll_wait($poll_ctx, 100); // 100ms timeout
echo "Events after write: " . count($events) . "\n";

foreach ($events as $event) {
    echo "Event: ";
    if ($event instanceof StreamPollEvent) {
        if ($event->events & STREAM_POLL_READ) {
            echo "Read event detected on: " . $event->data . "\n";
            // Read the data
            $data = fread($event->stream, 1024);
            echo "Events: " . $event->events . ", Read data: '$data'\n";
        }
        if ($event->events & STREAM_POLL_WRITE) {
            echo "Write event detected on: " . $event->data . "\n";
            echo "Events: " . $event->events . "\n";
        }
    }
}

// Clean up
fclose($socket1);
fclose($socket2);


?>
--EXPECT--
bool(true)
Events count: 1
Event: Events: 2, Data: socket2_data
Events after write: 2
Event: Write event detected on: socket2_data
Events: 2
Event: Read event detected on: socket1_data
Events: 1, Read data: 'test data'
