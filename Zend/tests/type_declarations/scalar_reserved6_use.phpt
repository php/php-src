--TEST--
Scalar type names cannot be used as class, trait or interface names (6) - use
--FILE--
<?php

use foobar as bool;
?>
--EXPECTF--
Fatal error: Cannot use foobar as bool because 'bool' is a special class name in %s on line %d
