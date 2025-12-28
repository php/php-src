--TEST--
Stream errors - error chaining
--FILE--
<?php

class TestStream {
    public $context;

    public function stream_open($path, $mode, $options, &$opened_path) {
        // This will generate multiple errors
        return false;
    }
}

stream_wrapper_register('test', 'TestStream');

$context = stream_context_create([
    'stream' => [
        'error_mode' => StreamErrorMode::Silent,
        'error_store' => StreamErrorStore::All,
    ]
]);

$stream = fopen('test://foo', 'r', false, $context);

$error = stream_get_last_error();
if ($error) {
    echo "Error count: " . $error->count() . "\n";
    echo "First error: " . $error->message . "\n";
    
    // Check if it has next error
    if ($error->next) {
        echo "Has chained error: yes\n";
    }
}

?>
--EXPECTF--
Error count: %d
First error: %s
%a
