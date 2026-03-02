--TEST--
Bug #79818: BIND_STATIC frees old variable value too early
--FILE--
<?php
function test($a) {
    static $a = UNDEFINED;
}
test(new stdClass);
?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant "UNDEFINED" in %s:%d
Stack trace:
#0 %s(%d): test(Object(stdClass))
#1 {main}
  thrown in %s on line %d
