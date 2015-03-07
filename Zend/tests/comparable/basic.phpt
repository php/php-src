--TEST--
Comparable: basic tests
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

echo "Comparing C to C:\n";
var_dump(new C(42) <=> new C(43));
var_dump(new C(42) <=> new C(42));
var_dump(new C(43) <=> new C(42));
echo "\n";

echo "Comparing C to integers:\n";
var_dump(new C(42) <=> 43);
var_dump(new C(42) <=> 42);
var_dump(new C(43) <=> 42);
var_dump(42 <=> new C(43));
var_dump(42 <=> new C(42));
var_dump(43 <=> new C(42));
echo "\n";

echo "Comparing C to nulls:\n";
var_dump(new C(42)  <=> null);
var_dump(new C(0)   <=> null);
var_dump(null <=> new C(42));
var_dump(null <=> new C(0));
?>
--EXPECTF--
Comparing C to C:
int(-1)
int(0)
int(1)

Comparing C to integers:
int(-1)
int(0)
int(1)
int(-1)
int(0)
int(1)

Comparing C to nulls:
int(1)
int(0)
int(-1)
int(0)
