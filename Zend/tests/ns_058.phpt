--TEST--
058: Usage of 'namespace' in compound names (out of namespase)
--FILE--
<?php
const C = "const ok\n";

function foo() {
	return "func ok\n";
}

class foo {
	const C = "const ok\n";
	const C2 = namespace\C;
	static $var = "var ok\n";
	function __construct() {
		echo "class ok\n";
	}
	static function bar() {
		return "method ok\n";
	}
}

function f1($x=namespace\C) {
	return $x;
}
function f2($x=namespace\foo::C) {
	return $x;
}

function f3(namespace\foo $x) {
	return "ok\n";
}

echo namespace\C;
echo namespace\foo();
echo namespace\foo::C;
echo namespace\foo::C2;
echo namespace\foo::$var;
echo namespace\foo::bar();
echo namespace\f1();
echo namespace\f2();
echo namespace\f3(new namespace\foo());
echo namespace\unknown;
?>
--EXPECTF--
const ok
func ok
const ok
const ok
var ok
method ok
const ok
const ok
class ok
ok

Fatal error: Undefined constant 'unknown' in %sns_058.php on line %d
