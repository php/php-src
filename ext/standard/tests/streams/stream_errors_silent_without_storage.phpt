--TEST--
Stream errors - error_store NONE option
--FILE--
<?php

$context = stream_context_create([
    'stream' => [
        'error_mode' => StreamErrorMode::Silent,
        'error_store' => StreamErrorStore::None,
    ]
]);

$stream = fopen('php://nonexistent', 'r', false, $context);

$errors = stream_last_errors();
echo "Has error: " . (!empty($error) ? "yes" : "no") . "\n";

?>
--EXPECT--
Has error: no
