--TEST--
Trying to extend a trait
--FILE--
<?php

trait abc { }

class foo extends abc { }

?>
--EXPECTF--
Fatal error: Class foo cannot extend trait abc in %s on line %d
