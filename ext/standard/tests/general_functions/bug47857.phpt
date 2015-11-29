--TEST--
Bug #47851 (is_callable throws fatal error)
--FILE--
<?php
class foo {
	function bar() {
		echo "ok\n";
	}
}
var_dump(is_callable(array('foo','bar')));
foo::bar();
var_dump(is_callable(array('Exception','getMessage')));
Exception::getMessage();
?>
--EXPECTF--
bool(true)

Deprecated: Non-static method foo::bar() should not be called statically in %sbug47857.php on line %d
ok
bool(false)

Fatal error: Uncaught Error: Non-static method Exception::getMessage() cannot be called statically in %sbug47857.php:%d
Stack trace:
#0 {main}
  thrown in %sbug47857.php on line %d

