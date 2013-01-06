--TEST--
Closure 046: Rebinding: preservation of previous scope when "static" given as scope arg (same as closure #041)
--FILE--
<?php

/* It's impossible to preserve the previous scope when doing so would break
 * the invariants that, for non-static closures, having a scope is equivalent
 * to having a bound instance. */

$nonstaticUnscoped = function () { var_dump(isset(A::$priv)); var_dump(isset($this)); };

class A {
	private static $priv = 7;
	function getClosure() {
		return function() { var_dump(isset(A::$priv)); var_dump(isset($this)); };
	}
}
class B extends A {}

$a = new A();
$nonstaticScoped = $a->getClosure();

echo "Before binding", "\n";
$nonstaticUnscoped(); echo "\n";
$nonstaticScoped(); echo "\n";

echo "After binding, no instance", "\n";
$d = $nonstaticUnscoped->bindTo(null, "static"); $d(); echo "\n";
$d = $nonstaticScoped->bindTo(null, "static"); $d(); echo "\n";
//$d should have been turned to static
$d->bindTo($d);

echo "After binding, with same-class instance for the bound one", "\n";
$d = $nonstaticUnscoped->bindTo(new A, "static"); $d(); echo "\n";
$d = $nonstaticScoped->bindTo(new A, "static"); $d(); echo "\n";

echo "After binding, with different instance for the bound one", "\n";
$d = $nonstaticScoped->bindTo(new B, "static"); $d(); echo "\n";

echo "Done.\n";

--EXPECTF--
Before binding
bool(false)
bool(false)

bool(true)
bool(true)

After binding, no instance
bool(false)
bool(false)

bool(true)
bool(false)


Warning: Cannot bind an instance to a static closure in %s on line %d
After binding, with same-class instance for the bound one
bool(false)
bool(true)

bool(true)
bool(true)

After binding, with different instance for the bound one
bool(true)
bool(true)

Done.
