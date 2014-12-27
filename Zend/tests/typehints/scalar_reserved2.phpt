--TEST--
Scalar type hint names cannot be used as class, trait or interface names (2)
--FILE--
<?php

class int {}
--EXPECTF--
Fatal error: Cannot declare class int because int is a type name in %s on line %d
