--TEST--
__toString() from trait with invalid return type
--FILE--
<?php

trait T {
    public function __toString(): int {
        return "ok";
    }
}

?>
--EXPECTF--
Fatal error: T::__toString(): Return type must be string when declared in %s on line %d
