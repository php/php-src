--TEST--
075: Redefining compile-time constants
--FILE--
<?php
namespace foo;
const NULL = 1;

echo NULL;
--EXPECTF--
Fatal error: Constant NULL cannot be redeclared in %s on line %d
