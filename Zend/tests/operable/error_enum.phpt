--TEST--
Unit enum cannot implement Operable
--FILE--
<?php

enum BadEnum implements Operable {
    case A;
}

?>
--EXPECTF--
Fatal error: Enum BadEnum cannot implement Operable in %s on line %d
