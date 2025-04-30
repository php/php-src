--TEST--
GH-17442 (Engine UAF with reference assign and dtor) - untyped
--CREDITS--
YuanchengJiang
--FILE--
<?php
$map = new WeakMap;
$obj = new stdClass;
$map[$obj] = new class {
    function __destruct() {
        throw new Exception("Test");
    }
};
headers_sent($obj,$generator);
?>
--EXPECTF--
Fatal error: Uncaught Exception: Test in %s:%d
Stack trace:
#0 [internal function]: class@anonymous->__destruct()
#1 %s(%d): headers_sent(NULL, 0)
#2 {main}
  thrown in %s on line %d
