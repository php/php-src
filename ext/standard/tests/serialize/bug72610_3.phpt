--TEST--
__wakeup should be able to modify dynamic properties without affecting copies of those properties
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip json_encode needed"; ?>
--FILE--
<?php

class Obj {
	// Testing $this->a being a dynamic property.

	function __construct($a) {
		$this->a = $a;
	}

	public function __wakeup() {
		echo "Calling __wakeup " . json_encode($this->a) . "\n";
		$this->a = "roh";
	}
}

function main() {
	$obj = new stdClass();
	$obj->c = null;
    $variable = [new Obj($obj), new Obj($obj), $obj];
	$serialized = serialize($variable);
	printf("%s\n", $serialized);
	$unserialized = unserialize($serialized);
	var_dump($unserialized);
}
main();
--EXPECTF--
a:3:{i:0;O:3:"Obj":1:{s:1:"a";O:8:"stdClass":1:{s:1:"c";N;}}i:1;O:3:"Obj":1:{s:1:"a";r:3;}i:2;r:3;}
Calling __wakeup {"c":null}
Calling __wakeup {"c":null}
array(3) {
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
  [2]=>
  object(stdClass)#%d (1) {
    ["c"]=>
    NULL
  }
}
