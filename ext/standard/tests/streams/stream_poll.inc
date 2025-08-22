<?php
// stream Poll Testing helper
function pt_new_socket_pair(): array {
    $domain = (strtoupper(substr(PHP_OS, 0, 3) == 'WIN') ? STREAM_PF_INET : STREAM_PF_UNIX);
    $sockets = stream_socket_pair($domain, STREAM_SOCK_STREAM, 0);
    if ($sockets === false) {
        die("Cannot create socket pair\n");
    }
    return $sockets;
}

function pt_new_tcp_socket_pair(): array {
    $server = stream_socket_server("tcp://127.0.0.1:0", $errno, $errstr);
    if (!$server) {
        die("Cannot create TCP server: $errstr\n");
    }
    $address = stream_socket_get_name($server, false);

    $client = stream_socket_client("tcp://$address", $errno, $errstr);
    if (!$client) {
        fclose($server);
        die("Cannot connect to TCP server: $errstr\n");
    }

    $server_conn = stream_socket_accept($server);
    if (!$server_conn) {
        fclose($server);
        fclose($client);
        die("Cannot accept connection\n");
    }

    // Close the listening socket (no longer needed)
    fclose($server);

    return [$client, $server_conn];
}

function pt_new_stream_poll(): StreamPollContext {
    $backend = getenv('STREAM_POLL_TEST_BACKEND');
    return stream_poll_create($backend === false ? STREAM_POLL_BACKEND_AUTO : $backend);
}

function pt_write_sleep($stream, $data, $delay = 10000): int|false {
    $result = fwrite($stream, $data, $delay);
    usleep($delay);
    return $result;
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
