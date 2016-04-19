--TEST--
anonymous class use ref
--FILE--
<?php
$glow = 50;
new class use(&$glow) {

	public function __construct() {
		var_dump($this->glow);
		$this->glow *= 2;
		var_dump($this->glow);
	}
};
var_dump($glow);
?>
--EXPECT--
int(50)
int(100)
int(100)
