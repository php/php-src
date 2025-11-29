--TEST--
Stream errors - exception mode for terminal errors
--FILE--
<?php

$context = stream_context_create([
    'stream' => [
        'error_mode' => STREAM_ERROR_MODE_EXCEPTION,
    ]
]);

try {
    $stream = fopen('php://nonexistent', 'r', false, $context);
} catch (StreamException $e) {
    echo "Caught: " . $e->getMessage() . "\n";
    echo "Code: " . $e->getCode() . "\n";
    echo "Wrapper: " . $e->getWrapperName() . "\n";
}

?>
--EXPECTF--
Caught: Failed to open stream: operation failed
Code: 36
Wrapper: PHP
