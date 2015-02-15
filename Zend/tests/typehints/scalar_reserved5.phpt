--TEST--
Scalar type hint names cannot be used as class, trait or interface names (5)
--FILE--
<?php

class boolean {}
--EXPECTF--
Fatal error: "boolean" cannot be used as a class name in %s on line %d
