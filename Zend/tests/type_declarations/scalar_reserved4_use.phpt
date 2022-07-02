--TEST--
Scalar type names cannot be used as class, trait or interface names (4) - use
--FILE--
<?php

use foobar as string;
?>
--EXPECTF--
Fatal error: Cannot use foobar as string because 'string' is a special class name in %s on line %d
