<?php
// stream Poll Testing helper

function pt_new_stream_poll(): Io\Poll\Context {
    $backend = getenv('POLL_TEST_BACKEND');
    if ($backend === false) {
        return new Io\Poll\Context(Io\Poll\Backend::Auto);
    }

    // Map string to enum case using match
    $backend_enum = match(strtolower($backend)) {
        'auto' => Io\Poll\Backend::Auto,
        'poll' => Io\Poll\Backend::Poll,
        'epoll' => Io\Poll\Backend::Epoll,
        'kqueue' => Io\Poll\Backend::Kqueue,
        'eventports' => Io\Poll\Backend::EventPorts,
        'wsapoll' => Io\Poll\Backend::WSAPoll,
        default => throw new ValueError("Unknown backend: $backend"),
    };

    return new Io\Poll\Context($backend_enum);
}

function pt_stream_poll_add($poll_ctx, $stream, $events, $data = null): Io\Poll\Watcher {
    return $poll_ctx->add(new StreamPollHandle($stream), $events, $data);
}

function pt_skip_for_backend($backend, $msg): void {
    $backends_to_skip = is_array($backend) ? $backend : array($backend);
    $current_backend = pt_new_stream_poll()->getBackend();
    if (in_array($current_backend->name, $backends_to_skip)) {
        die("skip backend {$current_backend->name} $msg\n");
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
    $result = fwrite($stream, $data);
    usleep($delay);
    return $result;
}

function pt_event_array_to_string(array $events): string {
    $names = [];
    foreach ($events as $event) {
        $names[] = $event->name;
    }
    return empty($names) ? 'NONE' : implode('|', $names);
}

function pt_print_events($watchers, $read_data = false): void {
    if (!is_array($watchers)) {
        die("Events must be an array\n");
    }
    echo "Events count: " . count($watchers) . "\n";
    foreach ($watchers as $i => $watcher) {
        if (!$watcher instanceof Io\Poll\Watcher) {
            die('Invalid event type');
        }
        echo "Event[$i]: " . pt_event_array_to_string($watcher->getTriggeredEvents()) . ", user data: " . $watcher->getData();
        if ($read_data && $watcher->hasTriggered(Io\Poll\Event::Read)) {
            $data = fread($watcher->getHandle()->getStream(), 1024);
            echo ", read data: '$data'";
        }
        echo "\n";
    }
}

function pt_expect_events($watchers, $expected, ?Io\Poll\Context $poll_ctx = null): void {
    if (!is_array($watchers)) {
        die("Events must be an array\n");
    }

    if (!is_array($expected)) {
        die("Expected events must be an array\n");
    }

    $event_count = count($watchers);
    $expected_count = count($expected);

    // Get the current backend for backend-specific expectations
    $backend = $poll_ctx ? $poll_ctx->getBackend()->name : 'unknown';

    if ($event_count !== $expected_count) {
        echo "Event count mismatch: got $event_count, expected $expected_count\n";
        pt_print_mismatched_events($watchers, $expected, [], $backend);
        return;
    }

    // Convert events to comparable format for matching
    $actual_events = [];
    foreach ($watchers as $watcher) {
        if (!$watcher instanceof Io\Poll\Watcher) {
            die('Invalid event type');
        }

        $event_data = [
            'events' => $watcher->getTriggeredEvents(),
            'data' => $watcher->getData(),
        ];

        $actual_events[] = $event_data;
    }

    // Resolve backend-specific expectations
    $resolved_expected = [];
    foreach ($expected as $exp_event) {
        $resolved_event = $exp_event;

        // Check if events field is a backend-specific map (associative array with string keys)
        if (isset($exp_event['events']) && is_array($exp_event['events'])) {
            // Check if it's a backend map (has string keys like 'default', 'Kqueue', etc.)
            $keys = array_keys($exp_event['events']);
            $is_backend_map = false;
            foreach ($keys as $key) {
                if (is_string($key)) {
                    $is_backend_map = true;
                    break;
                }
            }

            if ($is_backend_map) {
                // It's a backend-specific map, resolve it
                $resolved_event['events'] = pt_resolve_backend_specific_value($exp_event['events'], $backend);
            }
            // Otherwise it's already an array of Event enums, leave it as-is
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
            if (pt_events_equal($act_event['events'], $exp_event['events']) &&
                $act_event['data'] === $exp_event['data']) {

                // If read data is expected, check it
                if (isset($exp_event['read'])) {
                    $read_data = fread($watchers[$act_idx]->getHandle()->getStream(), 1024);
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
        pt_print_mismatched_events($watchers, $expected, $matched, $backend);
    }
}

function pt_events_equal($actual, $expected): bool {
    // Both should be arrays of Event enums
    if (!is_array($actual) || !is_array($expected)) {
        return false;
    }

    if (count($actual) !== count($expected)) {
        return false;
    }

    // Sort both arrays by event name for comparison
    $actual_names = array_map(fn($e) => $e->name, $actual);
    $expected_names = array_map(fn($e) => $e->name, $expected);
    sort($actual_names);
    sort($expected_names);

    return $actual_names === $expected_names;
}

function pt_resolve_backend_specific_value($backend_map, $current_backend) {
    // Direct backend match
    if (isset($backend_map[$current_backend])) {
        return $backend_map[$current_backend];
    }

    // Check for multi-backend keys (e.g., "Kqueue|EventPorts")
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

function pt_print_mismatched_events($actual_watchers, $expected_watchers, $matched = [], $backend_name = null): void {
    echo "Actual events:\n";
    foreach ($actual_watchers as $i => $watcher) {
        $match_status = isset($matched[$i]) ? " [MATCHED]" : " [UNMATCHED]";
        $watcher_names = pt_event_array_to_string($watcher->getTriggeredEvents());
        echo "  Event[$i]: $watcher_names, user data: " . $watcher->getData() . $match_status . "\n";
    }

    echo "Expected events:\n";
    foreach ($expected_watchers as $i => $exp_event) {
        $was_matched = in_array($i, $matched);
        $match_status = $was_matched ? " [MATCHED]" : " [UNMATCHED]";

        $events_value = $exp_event['events'];
        
        // Check if it's a backend-specific map
        if (is_array($events_value) && $backend_name !== null) {
            $keys = array_keys($events_value);
            $is_backend_map = false;
            foreach ($keys as $key) {
                if (is_string($key)) {
                    $is_backend_map = true;
                    break;
                }
            }
            
            if ($is_backend_map) {
                $events_value = pt_resolve_backend_specific_value($events_value, $backend_name);
            }
        }

        $watcher_names = is_array($events_value) ? pt_event_array_to_string($events_value) : 'INVALID';
        echo "  Event[$i]: $watcher_names, user data: " . $exp_event['data'];
        if (isset($exp_event['read'])) {
            echo ", read data: '" . $exp_event['read'] . "'";
        }
        echo $match_status . "\n";
    }
}
