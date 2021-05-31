--TEST--
Mismatched enum backing type
--FILE--
<?php

enum Foo: int {
    case Bar = 'bar';
}

?>
--EXPECTF--
Fatal error: Enum case type string does not match enum backing type int in %s on line %d
