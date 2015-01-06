--TEST--
Scalar type hint names cannot be used as class, trait or interface names (6) - use
--FILE--
<?php

use foobar as bool;
--EXPECTF--
Fatal error: "bool" cannot be used as a class name in %s on line %d
