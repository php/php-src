--TEST--
Bug #64280 (__destruct loop segfaults) - Basic behaviour
--FILE--
<?php
class getDestructed {
	public function __destruct() {
		new self;
	}
}
$class = new getDestructed;
?>
--EXPECTF--
Fatal error: Magic method or function too often called recursively in %s on line %d
