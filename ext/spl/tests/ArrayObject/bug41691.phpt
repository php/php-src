--TEST--
Bug #41691 (ArrayObject::exchangeArray hangs Apache)
--FILE--
<?php

class A extends ArrayObject {
    public function __construct($dummy, $flags) {
        parent::__construct($this, $flags);
    }
    public $a;
    public $b;
    public $c;
}

$a = new A(null, ArrayObject::ARRAY_AS_PROPS );
var_dump($a->exchangeArray(array('a'=>1,'b'=>1,'c'=>1)));

echo "Done\n";
?>
--EXPECTF--
Deprecated: ArrayObject::__construct(): Using an object as a backing array for ArrayObject is deprecated, as it allows violating class constraints and invariants in %s on line %d
array(3) {
  ["a"]=>
  NULL
  ["b"]=>
  NULL
  ["c"]=>
  NULL
}
Done
