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
Fatal error: Uncaught Error: Cannot declare self-referencing constant self::BAR in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
