--TEST--
Void cannot be nullable
--FILE--
<?php

function test() : ?void {
}

?>
--EXPECTF--
Fatal error: Void can only be used as a standalone type in %s on line %d
