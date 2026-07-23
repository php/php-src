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
        if (is_array($event)) {
            // Optional slot, rendered in brackets.
            $names[] = '[' . pt_event_array_to_string($event) . ']';
        } else {
            $names[] = $event->name;
        }
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

    // An expected item that is itself an array marks an optional slot: it may be
    // absent, or filled by exactly one of the events it lists. Plain enums are
    // required. This lets a backend accept e.g. Write with an optional HangUp
    // that may not have propagated yet, without weakening the required events.
    $required = [];
    $optional_groups = [];
    foreach ($expected as $item) {
        if (is_array($item)) {
            $optional_groups[] = array_map(fn($e) => $e->name, $item);
        } else {
            $required[] = $item->name;
        }
    }

    $actual_names = array_map(fn($e) => $e->name, $actual);

    // Every required event must be present.
    foreach ($required as $name) {
        $idx = array_search($name, $actual_names, true);
        if ($idx === false) {
            return false;
        }
        unset($actual_names[$idx]);
    }

    // Each leftover actual event must be accounted for by a distinct optional
    // slot that lists it.
    foreach ($actual_names as $name) {
        $matched = false;
        foreach ($optional_groups as $gi => $group) {
            if (in_array($name, $group, true)) {
                unset($optional_groups[$gi]);
                $matched = true;
                break;
            }
        }
        if (!$matched) {
            return false;
        }
    }

    return true;
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

/*
 * Self-test for the pt_* event-matching helpers.
 *
 * This is deliberately NOT a .phpt: the CI only runs .phpt files, never .inc,
 * so this never executes there. It only runs when poll.inc is executed as the
 * main script, which lets the (fiddly) optional-slot matching logic be verified
 * by hand:
 *
 *     sapi/cli/php ext/standard/tests/poll/poll.inc
 */
function pt_run_self_test(): void {
    if (!enum_exists('Io\\Poll\\Event')) {
        echo "Io\\Poll\\Event not available; build with poll support to run the self-test\n";
        return;
    }

    $R = Io\Poll\Event::Read;
    $W = Io\Poll\Event::Write;
    $E = Io\Poll\Event::Error;
    $H = Io\Poll\Event::HangUp;

    $failures = 0;
    $check = function (string $label, bool $cond) use (&$failures) {
        if ($cond) {
            echo "ok   - $label\n";
        } else {
            echo "FAIL - $label\n";
            ++$failures;
        }
    };

    // Plain required sets are order independent and exact.
    $check('exact set matches', pt_events_equal([$W, $H], [$H, $W]));
    $check('missing required fails', !pt_events_equal([$W], [$W, $H]));
    $check('extra unexpected fails', !pt_events_equal([$W, $H], [$W]));

    // Optional slot: [$W, [$H]] means Write required, HangUp allowed but not
    // required.
    $check('optional present matches', pt_events_equal([$W, $H], [$W, [$H]]));
    $check('optional absent matches', pt_events_equal([$W], [$W, [$H]]));
    $check('optional cannot satisfy a required event', !pt_events_equal([$H], [$W, [$H]]));
    $check('unlisted extra with optional fails', !pt_events_equal([$W, $E], [$W, [$H]]));

    // A single optional slot accepts at most one of its listed events; separate
    // slots can each accept one.
    $check('optional picks one of group', pt_events_equal([$W, $H], [$W, [$H, $E]]));
    $check('optional group may stay empty', pt_events_equal([$W], [$W, [$H, $E]]));
    $check('single slot rejects two', !pt_events_equal([$W, $H, $E], [$W, [$H, $E]]));
    $check('two slots accept two', pt_events_equal([$W, $H, $E], [$W, [$H], [$E]]));

    // Rendering.
    $check('renders plain set', pt_event_array_to_string([$W, $H]) === 'Write|HangUp');
    $check('renders optional slot', pt_event_array_to_string([$W, [$H]]) === 'Write|[HangUp]');
    $check('renders empty as NONE', pt_event_array_to_string([]) === 'NONE');

    echo $failures === 0
        ? "\nAll self-tests passed\n"
        : "\n$failures self-test(s) FAILED\n";
}

if (isset($_SERVER['SCRIPT_FILENAME'])
        && realpath($_SERVER['SCRIPT_FILENAME']) === __FILE__) {
    pt_run_self_test();
}
