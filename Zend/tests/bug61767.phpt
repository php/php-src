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
?>
--EXPECTF--
Error handler called (Undefined variable $undefined)

Fatal error: Uncaught ErrorException: Undefined variable $undefined in %sbug61767.php:%d
Stack trace:
#0 %sbug61767.php(%d): {closure}(%s, 'Undefined varia...', '%s', %d)
#1 {main}
  thrown in %sbug61767.php on line %d
Shutting down
Array
(
    [type] => 1
    [message] => %a
    [file] => %sbug61767.php
    [line] => %d
)
