--TEST--
GH-10695: Exceptions in register_shutdown_function() are caught by set_exception_handler()
--FILE--
<?php
set_exception_handler(function (\Throwable $exception) {
    echo 'Caught: ' . $exception->getMessage() . "\n";
});

register_shutdown_function(function () {
    echo "register_shutdown_function()\n";
    throw new \Exception('shutdown');
});
?>
--EXPECT--
register_shutdown_function()
Caught: shutdown
