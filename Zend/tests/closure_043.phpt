--TEST--
Closure 043: Scope/bounding combination invariants; static closures
--FILE--
<?php
/* Whether it's scoped or not, a static closure cannot have
 * a bound instance. It should also not be automatically converted
 * to a non-static instance when attempting to bind one */

$staticUnscoped = static function () { var_dump(isset(A::$priv)); var_dump(isset($this)); };

class A {
	private static $priv = 7;
	static function getStaticClosure() {
		return static function() { var_dump(isset(A::$priv)); var_dump(isset($this)); };
	}
}

$staticScoped = A::getStaticClosure();

echo "Before binding", "\n";
$staticUnscoped(); echo "\n";
$staticScoped(); echo "\n";

echo "After binding, null scope, no instance", "\n";
$d = $staticUnscoped->bindTo(null, null); $d(); echo "\n";
$d = $staticScoped->bindTo(null, null); $d(); echo "\n";

echo "After binding, null scope, with instance", "\n";
$d = $staticUnscoped->bindTo(new A, null);
$d = $staticScoped->bindTo(new A, null);

echo "After binding, with scope, no instance", "\n";
$d = $staticUnscoped->bindTo(null, 'A'); $d(); echo "\n";
$d = $staticScoped->bindTo(null, 'A'); $d(); echo "\n";

echo "After binding, with scope, with instance", "\n";
$d = $staticUnscoped->bindTo(new A, 'A');
$d = $staticScoped->bindTo(new A, 'A');

echo "Done.\n";

--EXPECTF--
Before binding
bool(false)
bool(false)

bool(true)
bool(false)

After binding, null scope, no instance
bool(false)
bool(false)

bool(false)
bool(false)

After binding, null scope, with instance

Warning: Cannot bind an instance to a static closure in %s on line %d

Warning: Cannot bind an instance to a static closure in %s on line %d
After binding, with scope, no instance
bool(true)
bool(false)

bool(true)
bool(false)

After binding, with scope, with instance

Warning: Cannot bind an instance to a static closure in %s on line %d

Warning: Cannot bind an instance to a static closure in %s on line %d
Done.
