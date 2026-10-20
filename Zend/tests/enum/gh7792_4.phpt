--TEST--
GH-7792 (Refer to enum as enum instead of class)
--FILE--
<?php

interface A {}

enum Foo implements A, A {}

?>
--EXPECTF--
Fatal error: Enum Foo cannot implement previously implemented interface A in %s on line %d
