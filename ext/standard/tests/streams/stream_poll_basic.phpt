--TEST--
Stream polling basic functionality
--SKIPIF--
<?php
if (!function_exists('stream_poll_create')) {
    die("skip stream polling not available");
}
?>
--FILE--
<?php

// Test 1: Create polling context
echo "=== Test 1: Create polling context ===\n";
$poll_ctx = stream_poll_create();
var_dump($poll_ctx instanceof StreamPollContext);

// Test 2: Create with parameters
echo "\n=== Test 2: Create with parameters ===\n";
$poll_ctx2 = stream_poll_create(512, STREAM_POLL_BACKEND_AUTO);
var_dump($poll_ctx2 instanceof StreamPollContext);

// Test 3: Backend name
echo "\n=== Test 3: Backend name ===\n";
$backend = stream_poll_backend_name($poll_ctx);
var_dump(is_string($backend));
echo "Backend: $backend\n";

// Test 4: Cannot construct directly
echo "\n=== Test 4: Cannot construct directly ===\n";
try {
    new StreamPollContext();
    echo "ERROR: Should not allow direct construction\n";
} catch (Error $e) {
    echo "OK: " . $e->getMessage() . "\n";
}

// Test 5: Create socket pair for testing
echo "\n=== Test 5: Socket pair operations ===\n";
$sockets = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
if ($sockets === false) {
    echo "SKIP: Cannot create socket pair\n";
    exit(0);
}

list($socket1, $socket2) = $sockets;

// Test 6: Add streams to poll
echo "\n=== Test 6: Add streams ===\n";
try {
    stream_poll_add($poll_ctx, $socket1, STREAM_POLL_READ, "socket1_data");
    stream_poll_add($poll_ctx, $socket2, STREAM_POLL_WRITE, "socket2_data");
    echo "OK: Added streams successfully\n";
} catch (StreamPollException $e) {
    echo "ERROR: " . $e->getMessage() . "\n";
}

// Test 7: Try to add same stream again (should fail)
echo "\n=== Test 7: Duplicate add (should fail) ===\n";
try {
    stream_poll_add($poll_ctx, $socket1, STREAM_POLL_READ);
    echo "ERROR: Should not allow duplicate add\n";
} catch (StreamPollException $e) {
    echo "OK: " . $e->getMessage() . "\n";
}

// Test 8: Modify stream events
echo "\n=== Test 8: Modify stream ===\n";
try {
    stream_poll_modify($poll_ctx, $socket1, STREAM_POLL_READ | STREAM_POLL_WRITE, "modified_data");
    echo "OK: Modified stream successfully\n";
} catch (StreamPollException $e) {
    echo "ERROR: " . $e->getMessage() . "\n";
}

// Test 9: Poll with timeout (should timeout immediately)
echo "\n=== Test 9: Poll with timeout ===\n";
try {
    $events = stream_poll_wait($poll_ctx, 0);
    var_dump(is_array($events));
    echo "Events count: " . count($events) . "\n";
    
    // Check if socket2 is ready for writing (it should be)
    foreach ($events as $event) {
        echo "Event: ";
        var_dump($event instanceof StreamPollEvent);
        if ($event instanceof StreamPollEvent) {
            echo "Events: " . $event->events . ", Data: " . $event->data . "\n";
        }
    }
} catch (StreamPollException $e) {
    echo "ERROR: " . $e->getMessage() . "\n";
}

// Test 10: Write data and poll for read
echo "\n=== Test 10: Write and poll ===\n";
fwrite($socket2, "test data");
try {
    $events = stream_poll_wait($poll_ctx, 100); // 100ms timeout
    echo "Events after write: " . count($events) . "\n";
    
    foreach ($events as $event) {
        if ($event instanceof StreamPollEvent) {
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
    }
} catch (StreamPollException $e) {
    echo "ERROR: " . $e->getMessage() . "\n";
}

// Test 11: Remove stream
echo "\n=== Test 11: Remove stream ===\n";
try {
    stream_poll_remove($poll_ctx, $socket1);
    echo "OK: Removed stream successfully\n";
} catch (StreamPollException $e) {
    echo "ERROR: " . $e->getMessage() . "\n";
}

// Test 12: Try to remove same stream again (should fail)
echo "\n=== Test 12: Remove non-existent stream ===\n";
try {
    stream_poll_remove($poll_ctx, $socket1);
    echo "ERROR: Should not allow removing non-existent stream\n";
} catch (StreamPollException $e) {
    echo "OK: " . $e->getMessage() . "\n";
}

// Test 13: Try to modify removed stream (should fail)
echo "\n=== Test 13: Modify removed stream ===\n";
try {
    stream_poll_modify($poll_ctx, $socket1, STREAM_POLL_READ);
    echo "ERROR: Should not allow modifying removed stream\n";
} catch (StreamPollException $e) {
    echo "OK: " . $e->getMessage() . "\n";
}

// Clean up
fclose($socket1);
fclose($socket2);

echo "\n=== Test completed ===\n";

?>
--EXPECT--
=== Test 1: Create polling context ===
bool(true)

=== Test 2: Create with parameters ===
bool(true)

=== Test 3: Backend name ===
bool(true)
Backend: %s

=== Test 4: Cannot construct directly ===
OK: Cannot directly construct StreamPollContext, use stream_poll_create() instead

=== Test 5: Socket pair operations ===

=== Test 6: Add streams ===
OK: Added streams successfully

=== Test 7: Duplicate add (should fail) ===
OK: Stream already added

=== Test 8: Modify stream ===
OK: Modified stream successfully

=== Test 9: Poll with timeout ===
bool(true)
Events count: %d

=== Test 10: Write and poll ===
Events after write: %d

=== Test 11: Remove stream ===
OK: Removed stream successfully

=== Test 12: Remove non-existent stream ===
OK: Failed to remove stream from poll

=== Test 13: Modify removed stream ===
OK: Stream not found

=== Test completed ===