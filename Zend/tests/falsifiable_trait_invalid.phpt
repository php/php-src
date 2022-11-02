--TEST--
__toBool() from trait with invalid return type
--FILE--
<?php

trait T {
    public function __toBool(): int {
        return true;
    }
}

?>
--EXPECTF--
Fatal error: T::__toBool(): Return type must be bool when declared in %s on line %d
