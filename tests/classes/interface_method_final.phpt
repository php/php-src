--TEST--
ZE2 An interface method cannot be final
--FILE--
<?php

class if_a {
    abstract final function err();
}

?>
--EXPECTF--
Fatal error: Cannot use the final modifier on an abstract class member in %s on line %d
