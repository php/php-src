--TEST--
ASSIGN_STATIC_PROP_REF result should have live range
--FILE--
<?php
class Test {
    public static $prop;
}
Test::$prop =& $v + UNDEF;
?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant "UNDEF" in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
