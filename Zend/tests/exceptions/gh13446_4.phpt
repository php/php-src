--TEST--
GH-13446: Exception handler isn't restored if stack is empty
--FILE--
<?php
function exception_handler() {
    echo "Handler\n";
    restore_exception_handler();
    restore_exception_handler();
}
set_exception_handler('exception_handler');

register_shutdown_function(function () {
    var_dump(set_exception_handler(null));
    restore_exception_handler();
});

throw new Exception();
?>
--EXPECT--
Handler
NULL
