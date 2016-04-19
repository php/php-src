--TEST--
anonymous class use global again
--FILE--
<?php
$glow = 50;
function thing($glow) {
	return new class use($glow) {
		public function __construct() {
			var_dump($this->glow);
		}
	};
}

thing(400);
thing(500);
?>
--EXPECT--
int(400)
int(500)
