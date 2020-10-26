--TEST--
Scalar type names cannot be used as class, trait or interface names (3) - use
--FILE--
<?php

use foobar as float;
?>
--EXPECTF--
Fatal error: Cannot import class foobar as "float", float is a reserved class nam in %s on line %d
