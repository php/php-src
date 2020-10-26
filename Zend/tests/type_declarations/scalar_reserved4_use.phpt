--TEST--
Scalar type names cannot be used as class, trait or interface names (4) - use
--FILE--
<?php

use foobar as string;
?>
--EXPECTF--
Fatal error: Cannot import class foobar as "string", string is a reserved class nam in %s on line %d
