--TEST--
RFC example: returned type does not match the type declaration
--FILE--
<?php

function get_config(): array {
    return 42;
}

get_config();
?>
--EXPECTF--
Fatal error: Uncaught TypeError: get_config(): Return value must be of type array, int returned in %s:%d
Stack trace:
#0 %s(%d): get_config()
#1 {main}
  thrown in %s on line %d
