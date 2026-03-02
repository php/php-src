--TEST--
GH-17442 (Engine UAF with reference assign and dtor) - typed
--CREDITS--
YuanchengJiang
nielsdos
--FILE--
<?php
$map = new WeakMap;

class Test {
	public stdClass|string $obj;
}

$test = new Test;
$test->obj = new stdClass;

$map[$test->obj] = new class {
    function __destruct() {
		global $test;
		var_dump($test->obj);
        throw new Exception("Test");
    }
};

headers_sent($test->obj);
?>
--EXPECTF--
string(0) ""

Fatal error: Uncaught Exception: Test in %s:%d
Stack trace:
#0 [internal function]: class@anonymous->__destruct()
#1 %s(%d): headers_sent('')
#2 {main}
  thrown in %s on line %d
