--TEST--
Closure 044: Scope/bounding combination invariants; non static closures
--FILE--
<?php
/* A non-static closure has a bound instance if it has a scope
 * and does't have an instance if it has no scope */

$nonstaticUnscoped = function () { var_dump(isset(A::$priv)); var_dump(isset($this)); };

class A {
	private static $priv = 7;
	function getClosure() {
		return function() { var_dump(isset(A::$priv)); var_dump(isset($this)); };
	}
}

$a = new A();
$nonstaticScoped = $a->getClosure();

echo "Before binding", "\n";
$nonstaticUnscoped(); echo "\n";
$nonstaticScoped(); echo "\n";

echo "After binding, null scope, no instance", "\n";
$d = $nonstaticUnscoped->bindTo(null, null); $d(); echo "\n";
$d = $nonstaticScoped->bindTo(null, null); $d(); echo "\n";

echo "After binding, null scope, with instance", "\n";
$d = $nonstaticUnscoped->bindTo(new A, null); $d(); echo "\n";
$d = $nonstaticScoped->bindTo(new A, null); $d(); echo "\n";

echo "After binding, with scope, no instance", "\n";
$d = $nonstaticUnscoped->bindTo(null, 'A'); $d(); echo "\n";
$d = $nonstaticScoped->bindTo(null, 'A'); $d(); echo "\n";

echo "After binding, with scope, with instance", "\n";
$d = $nonstaticUnscoped->bindTo(new A, 'A'); $d(); echo "\n";
$d = $nonstaticScoped->bindTo(new A, 'A'); $d(); echo "\n";

echo "Done.\n";

--EXPECTF--
Before binding
bool(false)
bool(false)

bool(true)
bool(true)

After binding, null scope, no instance
bool(false)
bool(false)

bool(false)
bool(false)

After binding, null scope, with instance
bool(false)
bool(true)

bool(false)
bool(true)

After binding, with scope, no instance
bool(true)
bool(false)

bool(true)
bool(false)

After binding, with scope, with instance
bool(true)
bool(true)

bool(true)
bool(true)

Done.
