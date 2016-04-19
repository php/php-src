--TEST--
anonymous class cannot use variable twice
--FILE--
<?php
$glow = 10;

new class {
	public function __construct() {
		new class use($glow, $this->glow) {};
	}
}


?>
--EXPECTF--
Fatal error: Cannot use property name glow twice in %s on line 6


