--TEST--
__wakeup can replace a copy of the object referring to the root node.
--FILE--
<?php
/* This bug never happened, but adding this test to make sure that further changes to unserialize don't allow freeing the root in __wakeup. */
class Obj {
	function __construct($a) {
		$this->a = $a;
	}

	public function __wakeup() {
		echo "Calling __wakeup\n";
		$this->a = "replaced";
	}
}

$a = new stdClass();
$a->obj = new Obj($a);;
$serialized = serialize($a);
printf("%s\n", $serialized);
$unserialized = unserialize($serialized);
var_dump($unserialized);
--EXPECTF--
O:8:"stdClass":1:{s:3:"obj";O:3:"Obj":1:{s:1:"a";r:1;}}
Calling __wakeup
object(stdClass)#%d (1) {
  ["obj"]=>
  object(Obj)#%d (1) {
    ["a"]=>
    string(8) "replaced"
  }
}
