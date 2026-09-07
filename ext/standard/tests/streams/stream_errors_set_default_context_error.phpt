--TEST--
Stream errors - prohibit setting error mode in default context
--FILE--
<?php

try {
    stream_context_set_default([
        'stream' => [
            'error_mode' => StreamErrorMode::Exception,
        ]
    ]);
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
Stream error handling options cannot be set on the default context
