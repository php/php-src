--TEST--
Self-referencing constant expression (part of a constant AST)
--XFAIL--
Not yet fixed, to be fixed for PHP 5.6
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

