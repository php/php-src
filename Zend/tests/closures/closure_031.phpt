--TEST--
Closure 031: Closure properties with custom error handlers
--FILE--
<?php
function foo($errno, $errstr, $errfile, $errline) {
    echo "Warning: $errstr\n";
}
set_error_handler('foo');
$foo = function() {
};
try {
    var_dump($foo->a);
} catch (Error $ex) {
    echo "Error: {$ex->getMessage()}\n";
}
?>
--EXPECT--
Warning: Undefined property: Closure::$a
NULL
