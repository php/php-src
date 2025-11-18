--TEST--
Stream errors - custom error handler
--FILE--
<?php

$handler_called = false;

$context = stream_context_create([
    'stream' => [
        'error_mode' => STREAM_ERROR_MODE_SILENT,
        'error_handler' => function($wrapper, $stream, $code, $message, $param) use (&$handler_called) {
            $handler_called = true;
            echo "Handler called\n";
            echo "Wrapper: $wrapper\n";
            echo "Code: $code\n";
            echo "Message: $message\n";
            echo "Param: " . ($param ?? 'null') . "\n";
        }
    ]
]);

$stream = fopen('php://nonexistent', 'r', false, $context);

var_dump($handler_called);

?>
--EXPECT--
Handler called
Wrapper: PHP
Code: 36
Message: Failed to open stream: operation failed
Param: php://nonexistent
bool(true)
