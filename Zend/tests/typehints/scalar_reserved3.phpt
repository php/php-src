--TEST--
Scalar type hint names cannot be used as class, trait or interface names (3)
--FILE--
<?php

class float {}
--EXPECTF--
Fatal error: Cannot declare class float because float is a type name in %s on line %d
