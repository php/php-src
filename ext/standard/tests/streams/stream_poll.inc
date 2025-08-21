<?php
// stream Poll Testing helper
function pt_new_socket_pair(): array {
    $sockets = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
    if ($sockets === false) {
        die("Cannot create socket pair\n");
    }
    return $sockets;
}

function pt_new_stream_poll(): StreamPollContext {
    $backend = getenv('STREAM_POLL_TEST_BACKEND');
    return stream_poll_create($backend === false ? STREAM_POLL_BACKEND_AUTO : $backend);
}

function pt_print_events($events, $read_data = false): void {
    if (!is_array($events)) {
        die("Events must be an array\n");
    }
    echo "Events count: " . count($events) . "\n";
    foreach ($events as $i => $event) {
        if (!$event instanceof StreamPollEvent) {
            die('Invalid event type');
        }
        echo "Event[$i]: " . $event->events . ", user data: " . $event->data;
        if ($read_data && $event->events & STREAM_POLL_READ) {
            $data = fread($event->stream, 1024);
            echo ", read data: '$data'";
        }
        echo "\n";
    }
}
