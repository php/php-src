--TEST--
Trying to implement a trait
--FILE--
<?php

trait abc { }

class foo implements abc { }

?>
--EXPECTF--
Fatal error: foo cannot implement abc - it is not an interface in %s on line %d
