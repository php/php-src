--TEST--
Stream errors - StreamError hasCode method
--FILE--
<?php

$context = stream_context_create([
    'stream' => [
        'error_mode' => StreamErrorMode::Silent,
    ]
]);

$stream = fopen('php://nonexistent', 'r', false, $context);

$error = stream_get_last_error();
if ($error) {
    echo "Has OpenFailed: " . ($error->hasCode(StreamError::CODE_OPEN_FAILED) ? 'yes' : 'no') . "\n";
    echo "Has NotFound: " . ($error->hasCode(StreamError::CODE_NOT_FOUND) ? 'yes' : 'no') . "\n";
}

?>
--EXPECT--
Has OpenFailed: yes
Has NotFound: no
