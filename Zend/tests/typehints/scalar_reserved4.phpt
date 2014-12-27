--TEST--
Scalar type hint names cannot be used as class, trait or interface names (4)
--FILE--
<?php

class string {}
--EXPECTF--
Fatal error: Cannot declare class string because string is a type name in %s on line %d
