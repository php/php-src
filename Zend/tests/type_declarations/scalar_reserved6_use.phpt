--TEST--
Scalar type names cannot be used as class, trait or interface names (6) - use
--FILE--
<?php

use foobar as bool;
?>
--EXPECTF--
Fatal error: Cannot import class foobar as "bool", bool is a reserved class nam in %s on line %d
