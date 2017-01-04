--TEST--
__wakeup should be able to add dynamic properties without affecting other dynamic properties
--FILE--
<?php

class Obj {
	// Testing $this->a being a dynamic property.

	function __construct($a) {
		$this->a = $a;
	}

	public function __wakeup() {
		echo "Calling __wakeup\n";
		for ($i = 0; $i < 10000; $i++) {
			$this->{'b' . $i} = 42;
		}
	}
}

function main() {
	$obj = new stdClass();
	$obj->test = 'foo';
	$variable = [new Obj($obj), new Obj($obj)];
	$serialized = serialize($variable);
	printf("%s\n", $serialized);
	$unserialized = unserialize($serialized);
	for ($i = 0; $i < 10000; $i++) {
		if ($unserialized[0]->{'b' . $i} !== 42) {
			echo "Fail 0 b$i\n";
			return;
		}
		if ($unserialized[1]->{'b' . $i} !== 42) {
			echo "Fail 1 b$i\n";
			return;
		}
		unset($unserialized[0]->{'b' . $i});
		unset($unserialized[1]->{'b' . $i});
	}
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
    object(stdClass)#%d (1) {
      ["test"]=>
      string(3) "foo"
    }
  }
  [1]=>
  object(Obj)#%d (1) {
    ["a"]=>
    object(stdClass)#%d (1) {
      ["test"]=>
      string(3) "foo"
    }
  }
}
