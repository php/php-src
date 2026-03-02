--TEST--
Stream errors - exception mode for terminal errors
--FILE--
<?php

$context = stream_context_create([
    'stream' => [
        'error_mode' => StreamErrorMode::Exception,
    ]
]);

try {
    $stream = fopen('php://nonexistent', 'r', false, $context);
} catch (StreamException $e) {
    echo "Caught: " . $e->getMessage() . "\n";
    echo "Code: " . $e->getCode() . "\n";
    
    $error = $e->getError();
    if ($error) {
        echo "Wrapper: " . $error->wrapperName . "\n";
    }
}

?>
--EXPECTF--
Caught: Failed to open stream: operation failed
Code: 36
Wrapper: PHP
