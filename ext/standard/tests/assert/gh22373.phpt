--TEST--
GH-22373: AST pretty-printing drops meaningful parentheses surrounding property access
--FILE--
<?php

class Foo {
	public static Closure $sf = strrev(...);

	public function __construct(
		public Closure $f = strrev(...),
	) {
		try {
			assert(($this->f)('abc') !== 'cba');
		} catch (Throwable $e) {
			echo $e::class, ': ', $e->getMessage(), "\n";
		}
		try {
			assert(($this?->f)('abc') !== 'cba');
		} catch (Throwable $e) {
			echo $e::class, ': ', $e->getMessage(), "\n";
		}
		try {
			assert((self::$sf)('abc') !== 'cba');
		} catch (Throwable $e) {
			echo $e::class, ': ', $e->getMessage(), "\n";
		}
	}
}

new Foo();

?>
--EXPECT--
AssertionError: assert(($this->f)('abc') !== 'cba')
AssertionError: assert(($this?->f)('abc') !== 'cba')
AssertionError: assert((self::$sf)('abc') !== 'cba')
