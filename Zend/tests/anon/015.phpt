--TEST--
anonymous class use global
--FILE--
<?php
$glow = 50;
new class use($glow) {
	public function __construct() {
		var_dump($this->glow);
	}
};
?>
--EXPECT--
int(50)
