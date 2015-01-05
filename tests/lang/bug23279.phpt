--TEST--
Bug #23279 (exception handler stops after first function call)
--FILE--
<?php
ob_start();
set_exception_handler('redirect_on_error');
echo "Hello World\n";
throw new Exception;

function redirect_on_error($e) {
    ob_end_clean();
    echo "Goodbye Cruel World\n";
}
?>
--EXPECT--
Goodbye Cruel World
