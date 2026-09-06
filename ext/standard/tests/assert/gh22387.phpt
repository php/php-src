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
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
	assert(!new (bar)());
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
	assert(!(bar)::m());
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
	assert(!(bar)::$p);
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
	assert(!(bar)::C);
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
	assert((baz)::class !== 'stdClass');
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
AssertionError: assert(!$foo instanceof (bar))
AssertionError: assert(!new (bar)())
AssertionError: assert(!(bar)::m())
AssertionError: assert(!(bar)::$p)
AssertionError: assert(!(bar)::C)
AssertionError: assert((baz)::class !== 'stdClass')
