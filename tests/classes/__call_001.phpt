--TEST--
ZE2 __call()
--FILE--
<?php

class Caller {
	public $x = array(1, 2, 3);

	function __call($m, $a) {
		echo "Method $m called:\n";
		var_dump($a);
		return $this->x;
	}
}

$foo = new Caller();
$a = $foo->test(1, '2', 3.4, true);
var_dump($a);

?>
--EXPECT--
Method test called:
array(4) {
  [0]=>
  int(1)
  [1]=>
  string(1) "2"
  [2]=>
  float(3.4)
  [3]=>
  bool(true)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
