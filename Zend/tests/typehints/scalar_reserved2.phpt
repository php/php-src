--TEST--
Scalar type hint names cannot be used as class, trait or interface names (2)
--FILE--
<?php

class int {}
--EXPECTF--
Fatal error: "int" cannot be used as a class name in %s on line %d
