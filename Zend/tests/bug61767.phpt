--TEST--
Bug #61767 (Shutdown functions not called in certain error situation)
--FILE--
<?php
set_error_handler(function($code, $msg, $file = null, $line = null) {
    echo "Error handler called ($msg)\n";
    throw new \ErrorException($msg, $code, 0, $file, $line);
});

register_shutdown_function(function(){
    echo "Shutting down\n";
    print_r(error_get_last());
});

//$undefined = null; // defined variable does not cause problems
$undefined->foo();
--EXPECTF--
Error handler called (Undefined variable: undefined)

Warning: Uncaught exception 'ErrorException' with message 'Undefined variable: undefined' in %sbug61767.php:13
Stack trace:
#0 %sbug61767.php(13): {closure}(8, 'Undefined varia...', '%s', 13, Array)
#1 {main}
  thrown in %sbug61767.php on line 13

Fatal error: Call to a member function foo() on a non-object in %sbug61767.php on line 13
Shutting down
Array
(
    [type] => 1
    [message] => Call to a member function foo() on a non-object
    [file] => %sbug61767.php
    [line] => 13
)
