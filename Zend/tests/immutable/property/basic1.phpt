--TEST--
Immutable modifier is supported on properties.
--FILE--
<?php
class A {
	public immutable $a;
	protected immutable $b;
	private immutable $c;
	public function __construct() {
		$this->a   = 'b';
		$this->b   = 'd';
		$this->c   = 'f';
		echo "Ok\n";
	}
}
new A;
?>
--EXPECT--

Ok
