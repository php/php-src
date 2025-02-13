--TEST--
GH-10695: Exceptions from output buffering handlers during shutdown are caught
--FILE--
<?php
set_exception_handler(function (\Throwable $exception) {
    echo 'Caught: ' . $exception->getMessage() . "\n";
});

ob_start(function () {
    throw new \Exception('ob_start');
});
?>
--EXPECT--
Caught: ob_start
