--TEST--
GH-22387: AST pretty-printing drops meaningful parentheses around RHS of instanceof
--FILE--
<?php

class Foo {
	public static $p = true;
	public const C = true;

	public static function m() {
		return true;
	}
}

$foo = new Foo();
const bar = 'Foo';
const baz = new stdClass();

try {
	assert(!$foo instanceof (bar));
} catch (AssertionError $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	assert(!new (bar)());
} catch (AssertionError $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	assert(!(bar)::m());
} catch (AssertionError $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	assert(!(bar)::$p);
} catch (AssertionError $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	assert(!(bar)::C);
} catch (AssertionError $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	assert((baz)::class !== 'stdClass');
} catch (AssertionError $e) {
	echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
assert(!$foo instanceof (bar))
assert(!new (bar)())
assert(!(bar)::m())
assert(!(bar)::$p)
assert(!(bar)::C)
assert((baz)::class !== 'stdClass')
