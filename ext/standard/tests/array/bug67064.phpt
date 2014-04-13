--TEST--
Bug #67064 ()
--FILE--
<?php
class Counter implements Countable {
	public function count($mode = COUNT_NORMAL) {
		var_dump($mode == COUNT_RECURSIVE);
		return 1;
	}
}

$counter = new Counter;
var_dump(count($counter, COUNT_RECURSIVE));
?>
--EXPECTF--
bool(true)
int(1)
