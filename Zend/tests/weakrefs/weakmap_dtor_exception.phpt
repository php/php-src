--TEST--
Exception during WeakMap destruction during shutdown
--FILE--
<?php
$map = new WeakMap;
$obj = new stdClass;
$map[$obj] = new class {
    function __destruct() {
        throw new Exception("Test");
    }
};
?>
--EXPECTF--
Fatal error: Uncaught Exception: Test in %s:%d
Stack trace:
#0 [internal function]: class@anonymous->__destruct()
#1 {main}
  thrown in %s on line %d
