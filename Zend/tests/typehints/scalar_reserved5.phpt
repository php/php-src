--TEST--
Scalar type hint names cannot be used as class, trait or interface names (5)
--FILE--
<?php

class boolean {}
--EXPECTF--
Fatal error: Cannot declare class boolean because boolean is a type name in %s on line %d
