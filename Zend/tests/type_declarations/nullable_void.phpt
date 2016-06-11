--TEST--
Void cannot be nullable
--FILE--
<?php

function test() : ?void {
}

?>
--EXPECTF--
Fatal error: Void type cannot be nullable in %s on line %d
