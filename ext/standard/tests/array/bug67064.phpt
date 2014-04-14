--TEST--
Bug #67064 (Countable interface prevents using 2nd parameter ($mode) of count() function)
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
