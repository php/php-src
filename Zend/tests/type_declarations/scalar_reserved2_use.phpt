--TEST--
Scalar type names cannot be used as class, trait or interface names (2) - use
--FILE--
<?php

use foobar as int;
?>
--EXPECTF--
Fatal error: Cannot use foobar as int because 'int' is a special class name in %s on line %d
