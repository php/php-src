--TEST--
Attribute arguments support only const expressions.
--FILE--
<?php

#[A1(foo())]
class C1 { }

?>
--EXPECTF--
Fatal error: Constant expression contains invalid operations in %s
