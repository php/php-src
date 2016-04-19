--TEST--
anonymous class use private
--FILE--
<?php
$glow = 50;
new class use($glow) {
	public function __construct() {
		var_dump($this);
	}
};
?>
--EXPECT--
object(class@anonymous)#1 (1) {
  ["glow":"class@anonymous":private]=>
  int(50)
}
