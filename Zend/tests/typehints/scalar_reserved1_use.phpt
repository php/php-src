--TEST--
Scalar type hint names cannot be used as class, trait or interface names (1) - use
--FILE--
<?php

use foobar as integer;
--EXPECTF--
Fatal error: "integer" cannot be used as a class name in %s on line %d
