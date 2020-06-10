--TEST--
Enum case in class
--FILE--
<?php

class Foo {
    case Bar;
}

?>
--EXPECTF--
Fatal error: Case can only be used in enums in %s on line %d
