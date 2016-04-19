--TEST--
anonymous class use prop no ref
--FILE--
<?php
$glow = "php7";

new class use($glow) {
	public function __construct() {
		new class use($this->glow) {
			public function __construct() {
				var_dump($this->glow);
				$this->glow .= " rocks";
				var_dump($this->glow);
			}
		};	
	}
};

var_dump($glow);
?>
--EXPECT--
string(4) "php7"
string(10) "php7 rocks"
string(4) "php7"


