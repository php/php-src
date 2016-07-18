--TEST--
__wakeup should be able to modify dynamic properties without affecting copies of those properties
--FILE--
<?php
error_reporting(E_ALL|E_STRICT);

class Obj {
	// Testing $this->a being a dynamic property.

	function __construct($a) {
		$this->a = $a;
	}

	public function __wakeup() {
		echo "Calling __wakeup\n";
		$this->a = "roh";
	}
}

function main() {
	$obj = (object)["test" => 'foo'];  // array (not a reference, but should be copied on write)
    $variable = [new Obj($obj), new Obj($obj)];
	$serialized = serialize($variable);
	printf("%s\n", $serialized);
	$unserialized = unserialize($serialized);
	var_dump($unserialized);
}
main();
--EXPECTF--
a:2:{i:0;O:3:"Obj":1:{s:1:"a";O:8:"stdClass":1:{s:4:"test";s:3:"foo";}}i:1;O:3:"Obj":1:{s:1:"a";r:3;}}
Calling __wakeup
Calling __wakeup
array(2) {
  [0]=>
  object(Obj)#%d (1) {
    ["a"]=>
    string(3) "roh"
  }
  [1]=>
  object(Obj)#%d (1) {
    ["a"]=>
    string(3) "roh"
  }
}
