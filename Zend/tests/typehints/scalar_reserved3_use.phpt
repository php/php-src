--TEST--
Scalar type hint names cannot be used as class, trait or interface names (3) - use
--FILE--
<?php

use foobar as float;
--EXPECTF--
Fatal error: Cannot use foobar as float because 'float' is a special class name in %s on line %d
