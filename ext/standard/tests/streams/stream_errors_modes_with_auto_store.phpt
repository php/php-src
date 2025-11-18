--TEST--
Stream errors - error_store AUTO mode behavior
--FILE--
<?php

// AUTO with ERROR mode should store NONE
$context1 = stream_context_create([
    'stream' => [
        'error_mode' => STREAM_ERROR_MODE_ERROR,
        'error_store' => STREAM_ERROR_STORE_AUTO,
    ]
]);

@fopen('php://nonexistent', 'r', false, $context1);
$errors1 = stream_get_errors('PHP');
echo "ERROR mode AUTO: " . count($errors1) . "\n";

// AUTO with EXCEPTION mode should store NON_TERM
$context2 = stream_context_create([
    'stream' => [
        'error_mode' => STREAM_ERROR_MODE_EXCEPTION,
        'error_store' => STREAM_ERROR_STORE_AUTO,
    ]
]);

try {
    fopen('php://nonexistent2', 'r', false, $context2);
} catch (StreamException $e) {}

$errors2 = stream_get_errors('PHP');
echo "EXCEPTION mode AUTO: " . count($errors2) . "\n";

// AUTO with SILENT mode should store ALL
$context3 = stream_context_create([
    'stream' => [
        'error_mode' => STREAM_ERROR_MODE_SILENT,
        'error_store' => STREAM_ERROR_STORE_AUTO,
    ]
]);

fopen('php://nonexistent3', 'r', false, $context3);
$errors3 = stream_get_errors('PHP');
echo "SILENT mode AUTO: " . count($errors3) . "\n";

?>
--EXPECTF--
ERROR mode AUTO: 0
EXCEPTION mode AUTO: %d
SILENT mode AUTO: %d
