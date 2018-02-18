--TEST--
Immutable modifier is supported on properties.
--FILE--
<?php
class A {
	public static immutable $a;
	public immutable $b;
	protected static immutable $c;
	protected immutable $d;
	private static immutable $e;
	private immutable $f;
	static immutable $g;
	var immutable $h;
	public function __construct() {
		static::$a = 'a';
		$this->b   = 'b';
		static::$c = 'c';
		$this->d   = 'd';
		static::$e = 'e';
		$this->f   = 'f';
		static::$g = 'g';
		$this->h   = 'h';
		echo "Ok\n";
	}
}
new A;
?>
--EXPECT--

Ok
