--TEST--
Basic autoload_call_function() function
--FILE--
<?php
function customAutoloader($class) {
    function test() {}
}
autoload_register_function('customAutoloader');

autoload_call_function('test');
var_dump(function_exists('test', false));
?>
--EXPECT--
bool(true)
