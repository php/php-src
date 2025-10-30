<?php
// stream Poll Testing helper

function pt_new_stream_poll(): PollContext {
    $backend = getenv('POLL_TEST_BACKEND');
    return new PollContext($backend === false ? PollBackend::Auto : PollBackend::from($backend));
}

function pt_stream_poll_add($poll_ctx, $stream, $events, $data = null): PollWatcher {
    return $poll_ctx->add(new StreamPollHandle($stream), $events, $data);
}

function pt_skip_for_backend($backend, $msg): void {
    $backends_to_skip = is_array($backend) ? $backend : array($backend);
    $current_backend = pt_new_stream_poll()->getBackend();
    if (in_array($current_backend->value, $backends_to_skip)) {
        die("skip backend $current_backend->value $msg\n");
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

function pt_print_events($watchers, $read_data = false): void {
    if (!is_array($watchers)) {
        die("Events must be an array\n");
    }
    echo "Events count: " . count($watchers) . "\n";
    foreach ($watchers as $i => $watcher) {
        if (!$watcher instanceof PollWatcher) {
            die('Invalid event type');
        }
        echo "Event[$i]: " . $watcher->getTriggeredEvents() . ", user data: " . $watcher->getData();
        if ($read_data && $watcher->hasTriggered(POLL_EVENT_READ)) {
            $data = fread($watcher->getHandle()->getStream(), 1024);
            echo ", read data: '$data'";
        }
        echo "\n";
    }
}

function pt_expect_events($watchers, $expected, ?PollContext $poll_ctx = null): void {
    if (!is_array($watchers)) {
        die("Events must be an array\n");
    }

    if (!is_array($expected)) {
        die("Expected events must be an array\n");
    }

    $event_count = count($watchers);
    $expected_count = count($expected);

    // Get the current backend for backend-specific expectations
    $backend = $poll_ctx ? $poll_ctx->getBackend() : 'unknown';

    if ($event_count !== $expected_count) {
        echo "Event count mismatch: got $event_count, expected $expected_count\n";
        pt_print_mismatched_events($watchers, $expected, [], $backend->value);
        return;
    }

    // Convert events to comparable format for matching
    $actual_events = [];
    foreach ($watchers as $watcher) {
        if (!$watcher instanceof PollWatcher) {
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

        if (isset($exp_event['events']) && is_array($exp_event['events'])) {
            $resolved_event['events'] = pt_resolve_backend_specific_value($exp_event['events'], $backend->value);
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
        pt_print_mismatched_events($watchers, $expected, $matched, $backend->value);
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
    if ($flags & POLL_EVENT_READ) $names[] = 'READ';
    if ($flags & POLL_EVENT_WRITE) $names[] = 'WRITE';
    if ($flags & POLL_EVENT_ERROR) $names[] = 'ERROR';
    if ($flags & POLL_EVENT_HUP) $names[] = 'HUP';

    return empty($names) ? 'NONE:' . $flags : implode('|', $names);
}

function pt_print_mismatched_events($actual_watchers, $expected_watchers, $matched = [], $backend_name = null): void {
    echo "Actual events:\n";
    foreach ($actual_watchers as $i => $watcher) {
        $match_status = isset($matched[$i]) ? " [MATCHED]" : " [UNMATCHED]";
        $watcher_names = pt_event_flags_to_string($watcher->getTriggeredEvents());
        echo "  Event[$i]: $watcher_names, user data: " . $watcher->getData() . $match_status . "\n";
    }

    echo "Expected events:\n";
    foreach ($expected_watchers as $i => $exp_event) {
        $was_matched = in_array($i, $matched);
        $match_status = $was_matched ? " [MATCHED]" : " [UNMATCHED]";

        $watchers_value = $exp_event['events'];
        if (is_array($watchers_value) && $backend_name !== null) {
            $watchers_value = pt_resolve_backend_specific_value($watchers_value, $backend_name);
        }

        $watcher_names = pt_event_flags_to_string($watchers_value);
        echo "  Event[$i]: $watcher_names, user data: " . $exp_event['data'];
        if (isset($exp_event['read'])) {
            echo ", read data: '" . $exp_event['read'] . "'";
        }
        echo $match_status . "\n";
    }
}
