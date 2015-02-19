--TEST--
Comparable: sorting
--FILE--
<?php

class C implements Comparable {
	public function __construct($value) {
		$this->value = $value;
	}

	public function compareTo($other) {
		if ($other instanceof C) {
			return $this->value <=> $other->value;
		}

		return $this->value <=> $other;
	}
}

$arr = [new C(42), new C(41), new C(43)];
sort($arr);
var_dump($arr);
?>
--EXPECTF--
array(3) {
  [0]=>
  object(C)#%d (1) {
    ["value"]=>
    int(41)
  }
  [1]=>
  object(C)#%d (1) {
    ["value"]=>
    int(42)
  }
  [2]=>
  object(C)#%d (1) {
    ["value"]=>
    int(43)
  }
}
