--TEST--
anonymous class use prop object member change acc allow
--FILE--
<?php
$glow = 10;

class Foo {
	public function method() {
		return $this->glow;
	}
}

new class use($glow) {
	public function __construct() {
		$foo = new class extends Foo use($this->glow) {
			protected $glow;
		};

		var_dump($foo->method());
	}
};

var_dump($glow);
?>
--EXPECT--
int(10)
int(10)

