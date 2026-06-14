--TEST--
GH-13446: Exception handler is restored after is has finished
--FILE--
<?php
function exception_handler($ex) {
    echo 'Exception caught: ', $ex->getMessage(), "\n";
}
set_exception_handler('exception_handler');

register_shutdown_function(function () {
    echo set_exception_handler(null), "\n";
    restore_exception_handler();
});

throw new Exception('Test');
?>
--EXPECT--
Exception caught: Test
exception_handler
