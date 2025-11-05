--TEST--
Backed enums: values() on empty enum returns []
--FILE--
<?php

enum A: string {}

var_dump(A::values());

?>
--EXPECT--
array(0) {
}

