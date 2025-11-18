--TEST--
Stream errors - silent mode with error storage
--FILE--
<?php

$context = stream_context_create([
    'stream' => [
        'error_mode' => STREAM_ERROR_MODE_SILENT,
    ]
]);

$stream = fopen('php://nonexistent', 'r', false, $context);
var_dump($stream);

$errors = stream_get_errors();
echo "Error count: " . count($errors) . "\n";
if (count($errors) > 0) {
    echo "First error code: " . $errors[0]['code'] . "\n";
    echo "First error wrapper: " . $errors[0]['wrapper'] . "\n";
}

?>
--EXPECT--
bool(false)
Error count: 1
First error code: 36
First error wrapper: PHP
