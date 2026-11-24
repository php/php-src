--TEST--
Backed enum cannot implement Operable
--FILE--
<?php

enum BadBackedEnum: int implements Operable {
    case A = 1;
}

?>
--EXPECTF--
Fatal error: Enum BadBackedEnum cannot implement Operable in %s on line %d
