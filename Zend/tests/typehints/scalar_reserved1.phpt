--TEST--
Scalar type hint names cannot be used as class, trait or interface names (1)
--FILE--
<?php

class integer {}
--EXPECTF--
Fatal error: Cannot declare class integer because integer is a type name in %s on line %d
