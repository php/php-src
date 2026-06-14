--TEST--
Cannot use static::FOO in constant expressions
--FILE--
<?php

const C = static::FOO;

?>
--EXPECTF--
Fatal error: "static::" is not allowed in compile-time constants in %s on line %d
