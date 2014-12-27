--TEST--
Scalar type hint names cannot be used as class, trait or interface names (6)
--FILE--
<?php

class bool {}
--EXPECTF--
Fatal error: Cannot declare class bool because bool is a type name in %s on line %d
