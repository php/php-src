--TEST--
Trying to instantiate a trait
--FILE--
<?php

trait abc { 
}

new abc;

?>
--EXPECTF--
Fatal error: Cannot instantiate trait abc in %s on line %d
