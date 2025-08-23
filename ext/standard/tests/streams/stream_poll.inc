<?php
// stream Poll Testing helper

function pt_new_stream_poll(): StreamPollContext {
    $backend = getenv('STREAM_POLL_TEST_BACKEND');
    return stream_poll_create($backend === false ? STREAM_POLL_BACKEND_AUTO : $backend);
}

function pt_skip_for_backend($backend, $msg): void {
    $backends_to_skip = is_array($backend) ? $backend : array($backend);
    $current_backend = stream_poll_backend_name(pt_new_stream_poll());
    if (in_array($current_backend, $backends_to_skip)) {
        die("skip backend $current_backend $msg\n");
    }
}

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

function pt_new_tcp_socket_connections(int $num_conns): array {
    $server = stream_socket_server("tcp://127.0.0.1:0", $errno, $errstr);
    if (!$server) {
        die("Cannot create TCP server: $errstr\n");
    }
    $address = stream_socket_get_name($server, false);

    $clients = [];
    $server_conns = [];
    for ($i = 0; $i < $num_conns; ++$i) {
        $clients[$i] = stream_socket_client("tcp://$address", $errno, $errstr);
        if (!$clients[$i]) {
            fclose($server);
            die("Cannot connect to TCP server: $errstr\n");
        }

        $server_conns[$i] = stream_socket_accept($server);
        if (!$server_conns[$i]) {
            fclose($server);
            die("Cannot accept connection\n");
        }
    }


    // Close the listening socket (no longer needed)
    fclose($server);

    return [$clients, $server_conns];
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
function pt_expect_events($events, $expected, $poll_ctx = null): void {
    if (!is_array($events)) {
        die("Events must be an array\n");
    }

    if (!is_array($expected)) {
        die("Expected events must be an array\n");
    }

    $event_count = count($events);
    $expected_count = count($expected);

    if ($event_count !== $expected_count) {
        echo "Event count mismatch: got $event_count, expected $expected_count\n";
        pt_print_mismatched_events($events, $expected);
        return;
    }

    // Get current backend name for backend-specific expectations
    $backend_name = $poll_ctx ? stream_poll_backend_name($poll_ctx) : 'unknown';

    // Convert events to comparable format for matching
    $actual_events = [];
    foreach ($events as $event) {
        if (!$event instanceof StreamPollEvent) {
            die('Invalid event type');
        }

        $event_data = [
            'events' => $event->events,
            'data' => $event->data
        ];

        $actual_events[] = $event_data;
    }

    // Resolve backend-specific expectations
    $resolved_expected = [];
    foreach ($expected as $exp_event) {
        $resolved_event = $exp_event;

        if (isset($exp_event['events']) && is_array($exp_event['events'])) {
            $resolved_event['events'] = pt_resolve_backend_specific_value($exp_event['events'], $backend_name);
        }

        $resolved_expected[] = $resolved_event;
    }

    // Try to match each expected event with an actual event
    $matched = [];
    $unmatched_expected = [];

    foreach ($resolved_expected as $exp_idx => $exp_event) {
        $found_match = false;

        foreach ($actual_events as $act_idx => $act_event) {
            if (isset($matched[$act_idx])) {
                continue; // Already matched
            }

            // Check if events and data match
            if ($act_event['events'] === $exp_event['events'] &&
                $act_event['data'] === $exp_event['data']) {

                // If read data is expected, check it
                if (isset($exp_event['read'])) {
                    $read_data = fread($events[$act_idx]->stream, 1024);
                    if ($read_data !== $exp_event['read']) {
                        continue; // Read data doesn't match
                    }
                }

                $matched[$act_idx] = $exp_idx;
                $found_match = true;
                break;
            }
        }

        if (!$found_match) {
            $unmatched_expected[] = $exp_event;
        }
    }

    // Check if all events matched
    if (count($matched) === $event_count && empty($unmatched_expected)) {
        echo "Events matched - count: $event_count\n";
    } else {
        echo "Events did not match:\n";
        pt_print_mismatched_events($events, $resolved_expected, $matched);
    }
}

function pt_resolve_backend_specific_value($backend_map, $current_backend) {
    // Direct backend match
    if (isset($backend_map[$current_backend])) {
        return $backend_map[$current_backend];
    }

    // Check for multi-backend keys (e.g., "poll|epoll")
    foreach ($backend_map as $key => $value) {
        if (strpos($key, '|') !== false) {
            $backends = array_map('trim', explode('|', $key));
            if (in_array($current_backend, $backends)) {
                return $value;
            }
        }
    }

    // Fall back to default
    if (isset($backend_map['default'])) {
        return $backend_map['default'];
    }

    // If no match found, this is an error
    die("No backend-specific value found for '$current_backend' and no default specified\n");
}

function pt_event_flags_to_string($flags): string {
    $names = [];
    if ($flags & STREAM_POLL_READ) $names[] = 'READ';
    if ($flags & STREAM_POLL_WRITE) $names[] = 'WRITE';
    if ($flags & STREAM_POLL_ERROR) $names[] = 'ERROR';
    if ($flags & STREAM_POLL_HUP) $names[] = 'HUP';

    return empty($names) ? 'NONE' : implode('|', $names);
}

function pt_print_mismatched_events($actual_events, $expected_events, $matched = []): void {
    echo "Actual events:\n";
    foreach ($actual_events as $i => $event) {
        $match_status = isset($matched[$i]) ? " [MATCHED]" : " [UNMATCHED]";
        $event_names = pt_event_flags_to_string($event->events);
        echo "  Event[$i]: $event_names, user data: " . $event->data . $match_status . "\n";
    }

    echo "Expected events:\n";
    foreach ($expected_events as $i => $exp_event) {
        $was_matched = in_array($i, $matched);
        $match_status = $was_matched ? " [MATCHED]" : " [UNMATCHED]";
        $event_names = pt_event_flags_to_string($exp_event['events']);
        echo "  Event[$i]: $event_names, user data: " . $exp_event['data'];
        if (isset($exp_event['read'])) {
            echo ", read data: '" . $exp_event['read'] . "'";
        }
        echo $match_status . "\n";
    }
}
