--TEST--
ZE2 pre increment/decrement property of overloaded object with assignment
--FILE--
<?php 

class Test {
	private $real_a = 2;
	
	function __set($property, $value) {
	  if ($property = "a") {
	    $this->real_a = $value;
	  }
	}

	function __get($property) {
	  if ($property = "a") {
	    return $this->real_a;
	  }
	}
}

$obj = new Test;
var_dump($obj->a);
$t1 = ++$obj->a;
var_dump($obj->a);
echo "---Done---\n";
?>
--EXPECT--
int(2)
int(3)
---Done---
