--TEST--
Stream errors - StreamErrorCode helper methods
--FILE--
<?php

$context = stream_context_create([
    'stream' => [
        'error_mode' => StreamErrorMode::Silent,
    ]
]);

// Generate a network error
$stream = @fsockopen('invalid-host-12345.example.com', 80, $errno, $errstr, 1, $context);

$error = stream_get_last_error();
if ($error) {
    echo "Is I/O error: " . ($error->code->isIoError() ? 'yes' : 'no') . "\n";
    echo "Is filesystem error: " . ($error->code->isFileSystemError() ? 'yes' : 'no') . "\n";
    echo "Is network error: " . ($error->code->isNetworkError() ? 'yes' : 'no') . "\n";
    echo "Is wrapper error: " . ($error->code->isWrapperError() ? 'yes' : 'no') . "\n";
}

?>
--EXPECTF--
Is I/O error: %s
Is filesystem error: no
Is network error: %s
Is wrapper error: %s
