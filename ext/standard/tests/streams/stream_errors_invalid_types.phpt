--TEST--
Stream errors - invalid enum type throws TypeError
--FILE--
<?php

try {
    $context = stream_context_create([
        'stream' => [
            'error_mode' => 'invalid',
        ]
    ]);
    
    fopen('php://nonexistent', 'r', false, $context);
} catch (TypeError $e) {
    echo "Caught TypeError for error_mode\n";
}

try {
    $context = stream_context_create([
        'stream' => [
            'error_store' => 123,
        ]
    ]);
    
    fopen('php://nonexistent', 'r', false, $context);
} catch (TypeError $e) {
    echo "Caught TypeError for error_store\n";
}

?>
--EXPECTF--

Warning: fopen(php://nonexistent): Failed to open stream: operation failed in %s on line %d
Caught TypeError for error_mode

Warning: fopen(php://nonexistent): Failed to open stream: operation failed in %s on line %d
Caught TypeError for error_store
