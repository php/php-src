--TEST--
Stream errors - custom error handler
--FILE--
<?php

$handler_called = false;

$context = stream_context_create([
    'stream' => [
        'error_mode' => StreamErrorMode::Silent,
        'error_handler' => function(StreamError $error) use (&$handler_called) {
            $handler_called = true;
            echo "Handler called\n";
            echo "Wrapper: " . $error->wrapperName . "\n";
            echo "Code: " . $error->code->name . "\n";
            echo "Message: " . $error->message . "\n";
            echo "Param: " . ($error->param ?? 'null') . "\n";
            echo "Terminating: " . ($error->terminating ? 'yes' : 'no') . "\n";
        }
    ]
]);

$stream = fopen('php://nonexistent', 'r', false, $context);

var_dump($handler_called);

?>
--EXPECT--
Handler called
Wrapper: PHP
Code: OpenFailed
Message: Failed to open stream: operation failed
Param: php://nonexistent
Terminating: yes
bool(true)
