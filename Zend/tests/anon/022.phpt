--TEST--
anonymous class use prop object
--FILE--
<?php
$glow = new stdClass;
$glow->php7 = "php7";

new class use($glow) {
	public function __construct() {
		new class use($this->glow) {
			public function __construct() {
				var_dump($this->glow);
				$this->glow->rocks = " rocks";
				var_dump($this->glow);
			}
		};
	}
};

var_dump($glow);
?>
--EXPECT--
object(stdClass)#1 (1) {
  ["php7"]=>
  string(4) "php7"
}
object(stdClass)#1 (2) {
  ["php7"]=>
  string(4) "php7"
  ["rocks"]=>
  string(6) " rocks"
}
object(stdClass)#1 (2) {
  ["php7"]=>
  string(4) "php7"
  ["rocks"]=>
  string(6) " rocks"
}



