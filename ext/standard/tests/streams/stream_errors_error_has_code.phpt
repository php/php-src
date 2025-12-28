--TEST--
Stream errors - multiple operations stored
--FILE--
<?php

$context = stream_context_create([
    'stream' => [
        'error_mode' => StreamErrorMode::Silent,
        'error_store' => StreamErrorStore::All,
    ]
]);

// First operation
$stream1 = fopen('php://nonexistent1', 'r', false, $context);
$error1 = stream_get_last_error();

// Second operation
$stream2 = fopen('php://nonexistent2', 'r', false, $context);
$error2 = stream_get_last_error();

// Should get the most recent error (second operation)
if ($error2) {
    echo "Got most recent error\n";
    echo "Param contains 'nonexistent2': " . (strpos($error2->param ?? '', 'nonexistent2') !== false ? 'yes' : 'no') . "\n";
}

?>
--EXPECT--
Got most recent error
Param contains 'nonexistent2': yes
