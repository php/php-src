--TEST--
GH-10695: Exceptions in error handlers during shutdown are caught
--FILE--
<?php
set_exception_handler(function (\Throwable $exception) {
    echo 'Caught: ' . $exception->getMessage() . "\n";
});
set_error_handler(function ($errno, $errstr) {
    throw new \Exception($errstr);
});
register_shutdown_function(function () {
    trigger_error('main', E_USER_ERROR);
});
?>
--EXPECT--
Caught: main
