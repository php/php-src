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
		} catch (Error $e) {
			echo $e->getMessage(), PHP_EOL;
		}
		try {
			assert(($this?->f)('abc') !== 'cba');
		} catch (Error $e) {
			echo $e->getMessage(), PHP_EOL;
		}
		try {
			assert((self::$sf)('abc') !== 'cba');
		} catch (Error $e) {
			echo $e->getMessage(), PHP_EOL;
		}
	}
}

new Foo();

?>
--EXPECT--
assert(($this->f)('abc') !== 'cba')
assert(($this?->f)('abc') !== 'cba')
assert((self::$sf)('abc') !== 'cba')
