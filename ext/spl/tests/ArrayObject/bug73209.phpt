--TEST--
Bug #73209: RecursiveArrayIterator does not iterate object properties
--FILE--
<?php

class hello {
  public $props = array();
  function __construct() {
    $this->props = ['hello' => 5, 'props' => ['keyme' => ['test' => 5]]];
  }
}
$data = new hello();

$iterator = new RecursiveIteratorIterator(new RecursiveArrayIterator($data), RecursiveIteratorIterator::SELF_FIRST);
echo "Expect to see all keys in ->props here: \n";

foreach($iterator as $k=>$v) {
    echo $k . "\n";
}

?>
--EXPECTF--
Deprecated: ArrayIterator::__construct(): Using an object as a backing array for ArrayIterator is deprecated, as it allows violating class constraints and invariants in %s on line %d
Expect to see all keys in ->props here: 
props
hello
props
keyme
test
