--TEST--
Stream errors - StreamError helper methods
--FILE--
<?php

$context = stream_context_create([
    'stream' => [
        'error_mode' => StreamErrorMode::Silent,
    ]
]);

// Generate a network error
$stream = fopen('php://nonexistent', 'r', false, $context);

$error = stream_get_last_error();
if ($error) {
    echo "Is I/O error: " . ($error->isIoError() ? 'yes' : 'no') . "\n";
    echo "Is filesystem error: " . ($error->isFileSystemError() ? 'yes' : 'no') . "\n";
    echo "Is network error: " . ($error->isNetworkError() ? 'yes' : 'no') . "\n";
    echo "Is wrapper error: " . ($error->isWrapperError() ? 'yes' : 'no') . "\n";
}

?>
--EXPECTF--
Is I/O error: no
Is filesystem error: yes
Is network error: no
Is wrapper error: no
