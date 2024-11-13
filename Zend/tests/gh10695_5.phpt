--TEST--
GH-10695: Exception handlers can register another exception handler
--FILE--
<?php
set_exception_handler(function (\Throwable $exception) {
    echo 'Caught: ' . $exception->getMessage() . "\n";
    set_exception_handler(function (\Throwable $exception) {
        echo 'Caught: ' . $exception->getMessage() . "\n";
    });
    throw new \Exception('exception handler');
});

throw new \Exception('main');
?>
--EXPECT--
Caught: main
Caught: exception handler
