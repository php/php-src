--TEST--
Stream polling basic functionality - socket write
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

stream_poll_add($poll_ctx, $socket1, STREAM_POLL_READ, "socket_data");

fwrite($socket2, "test data");
$events = stream_poll_wait($poll_ctx, 100); // 100ms timeout
echo "Events after write: " . count($events) . "\n";

foreach ($events as $event) {
    if ($event->events & STREAM_POLL_READ) {
        echo "Read event detected on: " . $event->data . "\n";
        // Read the data
        $data = fread($event->stream, 1024);
        echo "Read data: '$data'\n";
    }
    if ($event->events & STREAM_POLL_WRITE) {
        echo "Write event detected on: " . $event->data . "\n";
    }
}

// Clean up
fclose($socket1);
fclose($socket2);


?>
--EXPECT--
Events after write: 1
Read event detected on: socket_data
Read data: 'test data'
