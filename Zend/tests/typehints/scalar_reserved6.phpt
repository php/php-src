--TEST--
Scalar type hint names cannot be used as class, trait or interface names (6)
--FILE--
<?php

class bool {}
--EXPECTF--
Fatal error: "bool" cannot be used as a class name in %s on line %d
