--TEST--
spl_autoload_register_function_loader() rejects spl_autoload_call_function_loader as callback
--FILE--
<?php
try {
    spl_autoload_register_function_loader('spl_autoload_call_function_loader');
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
spl_autoload_register_function_loader(): Argument #1 ($callback) must not be the spl_autoload_call_function_loader() function
