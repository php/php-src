--TEST--
Self-referencing constant expression (part of a constant AST)
--FILE--
<?php
class A {
   const FOO = [self::BAR];
   const BAR = [self::FOO];
}
var_dump(A::FOO);
?>
--EXPECTF--
Fatal error: Cannot declare self-referencing constant 'self::FOO' in %s on line %d

