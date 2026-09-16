--TEST--
Stream errors - silent mode with error storage
--FILE--
<?php

$context = stream_context_create([
    'stream' => [
        'error_mode' => StreamErrorMode::Silent,
    ]
]);

$stream = fopen('php://nonexistent', 'r', false, $context);
var_dump($stream);

$errors = stream_last_errors();
foreach ($errors as $error) {
    echo "Has error: yes\n";
    echo "Error code: " . $error->code->name . "\n";
    echo "Error wrapper: " . $error->wrapperName . "\n";
    echo "Error message: " . $error->message . "\n";
}

?>
--EXPECT--
bool(false)
Has error: yes
Error code: OpenFailed
Error wrapper: PHP
Error message: Failed to open stream: operation failed
