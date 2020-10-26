--TEST--
Scalar type names cannot be used as class, trait or interface names (2) - use
--FILE--
<?php

use foobar as int;
?>
--EXPECTF--
Fatal error: Cannot import class foobar as "int", int is a reserved class nam in %s on line %d
