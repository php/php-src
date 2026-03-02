--TEST--
Stream errors - error mode with standard error reporting
--FILE--
<?php

$context = stream_context_create([
    'stream' => [
        'error_mode' => StreamErrorMode::Error,
    ]
]);

// This will trigger a warning
$stream = fopen('php://nonexistent', 'r', false, $context);

var_dump($stream);

?>
--EXPECTF--
Warning: fopen(php://nonexistent): Failed to open stream: %s in %s on line %d
bool(false)
