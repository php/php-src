--TEST--
Stream errors - error_store NONE option
--FILE--
<?php

$context = stream_context_create([
    'stream' => [
        'error_mode' => STREAM_ERROR_MODE_SILENT,
        'error_store' => STREAM_ERROR_STORE_NONE,
    ]
]);

$stream = fopen('php://nonexistent', 'r', false, $context);

$errors = stream_get_errors();
echo "Error count: " . count($errors) . "\n";

?>
--EXPECT--
Error count: 0
