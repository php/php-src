--TEST--
Scalar type hint names cannot be used as class, trait or interface names (4)
--FILE--
<?php

class string {}
--EXPECTF--
Fatal error: "string" cannot be used as a class name in %s on line %d
