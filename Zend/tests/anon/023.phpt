--TEST--
anonymous class use prop object member object
--FILE--
<?php
$glow = new stdClass;
$glow->php7 = new stdClass;

new class use($glow->php7) {
	public function __construct() {
		new class use($this->php7) {
			public function __construct() {
				var_dump($this->php7);
				$this->php7->rocks = " rocks";
				var_dump($this->php7);
				var_dump($this);
			}
		};
	}
};

var_dump($glow);
?>
--EXPECT--
object(stdClass)#2 (0) {
}
object(stdClass)#2 (1) {
  ["rocks"]=>
  string(6) " rocks"
}
object(class@anonymous)#4 (1) {
  ["php7":"class@anonymous":private]=>
  object(stdClass)#2 (1) {
    ["rocks"]=>
    string(6) " rocks"
  }
}
object(stdClass)#1 (1) {
  ["php7"]=>
  object(stdClass)#2 (1) {
    ["rocks"]=>
    string(6) " rocks"
  }
}
