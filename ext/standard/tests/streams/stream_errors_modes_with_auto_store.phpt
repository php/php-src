--TEST--
Stream errors - error_store AUTO mode behavior
--FILE--
<?php

// AUTO with ERROR mode should store NONE
$context1 = stream_context_create([
    'stream' => [
        'error_mode' => StreamErrorMode::Error,
        'error_store' => StreamErrorStore::Auto,
    ]
]);

@fopen('php://nonexistent', 'r', false, $context1);
$error1 = stream_get_last_error();
echo "ERROR mode AUTO: " . ($error1 ? "has error" : "no error") . "\n";

// AUTO with EXCEPTION mode should store NON_TERM
$context2 = stream_context_create([
    'stream' => [
        'error_mode' => StreamErrorMode::Exception,
        'error_store' => StreamErrorStore::Auto,
    ]
]);

try {
    fopen('php://nonexistent2', 'r', false, $context2);
} catch (StreamException $e) {}

$error2 = stream_get_last_error();
echo "EXCEPTION mode AUTO: " . ($error2 ? "has error" : "no error") . "\n";

// AUTO with SILENT mode should store ALL
$context3 = stream_context_create([
    'stream' => [
        'error_mode' => StreamErrorMode::Silent,
        'error_store' => StreamErrorStore::Auto,
    ]
]);

fopen('php://nonexistent3', 'r', false, $context3);
$error3 = stream_get_last_error();
echo "SILENT mode AUTO: " . ($error3 ? "has error" : "no error") . "\n";

?>
--EXPECTF--
ERROR mode AUTO: no error
EXCEPTION mode AUTO: %s
SILENT mode AUTO: has error
