--TEST--
ArrayObject: the destructor of a replaced value can share and write to the storage
--FILE--
<?php
class D {
    public function __construct(private ArrayObject $ao, private string|int $key) {}
    public function __destruct() {
        global $copy;
        $copy = $this->ao->__serialize()[1];
        $this->ao[$this->key] = 3;
    }
}

$ao = new ArrayObject(['a' => 1, 'b' => 2]);
$ao['a'] = new D($ao, 'c');
$ao['a'] = 'new';
var_dump($ao->getArrayCopy(), $copy);

$ao = new ArrayObject([1, 2]);
$ao[0] = new D($ao, 2);
$ao[0] = 'new';
var_dump($ao->getArrayCopy(), $copy);
?>
--EXPECT--
array(3) {
  ["a"]=>
  string(3) "new"
  ["b"]=>
  int(2)
  ["c"]=>
  int(3)
}
array(2) {
  ["a"]=>
  string(3) "new"
  ["b"]=>
  int(2)
}
array(3) {
  [0]=>
  string(3) "new"
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(2) {
  [0]=>
  string(3) "new"
  [1]=>
  int(2)
}
