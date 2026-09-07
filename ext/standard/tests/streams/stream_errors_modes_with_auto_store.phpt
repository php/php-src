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
$errors1 = stream_last_errors();
echo "ERROR mode AUTO: " . (!empty($errors1) ? "has error" : "no error") . "\n";

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

$errors2 = stream_last_errors();
echo "EXCEPTION mode AUTO: " . (!empty($errors2) ? "has error" : "no error") . "\n";

// AUTO with SILENT mode should store ALL
$context3 = stream_context_create([
    'stream' => [
        'error_mode' => StreamErrorMode::Silent,
        'error_store' => StreamErrorStore::Auto,
    ]
]);

fopen('php://nonexistent3', 'r', false, $context3);
$errors3 = stream_last_errors();
echo "SILENT mode AUTO: " . (!empty($errors3) ? "has error" : "no error") . "\n";

?>
--EXPECTF--
ERROR mode AUTO: no error
EXCEPTION mode AUTO: %s
SILENT mode AUTO: has error
