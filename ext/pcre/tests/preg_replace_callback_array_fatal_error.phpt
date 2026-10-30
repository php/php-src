--TEST--
preg_replace_callback_array() invalid callable
--FILE--
<?php

function b() {
    return "b";
}

// invalid callable
var_dump(preg_replace_callback_array(
    array(
        "/a/" => 'b',
        "/b/" => 'invalid callable'), 'a'));

?>
--EXPECTF--
Fatal error: Uncaught TypeError: preg_replace_callback_array(): Argument #1 ($pattern) must contain only valid callbacks in %spreg_replace_callback_array_fatal_error.php:%d
Stack trace:
#0 %spreg_replace_callback_array_fatal_error.php(%d): preg_replace_callback_array(Array, 'a')
#1 {main}
  thrown in %spreg_replace_callback_array_fatal_error.php on line %d
