--TEST--
anonymous class use counted
--FILE--
<?php
$glow = "php7";
new class use(&$glow) {

	public function __construct() {
		var_dump($this->glow);
		$this->glow .= " rocks";
		var_dump($this->glow);
	}
};

var_dump($glow);
?>
--EXPECT--
string(4) "php7"
string(10) "php7 rocks"
string(10) "php7 rocks"

