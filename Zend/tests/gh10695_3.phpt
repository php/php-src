--TEST--
GH-10695: Uncaught exceptions are not caught again
--FILE--
<?php
register_shutdown_function(function () {
    echo "shutdown\n";
    set_exception_handler(function (\Throwable $exception) {
        echo 'Caught: ' . $exception->getMessage() . "\n";
    });
});

throw new \Exception('main');
?>
--EXPECTF--
Fatal error: Uncaught Exception: main in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
shutdown
