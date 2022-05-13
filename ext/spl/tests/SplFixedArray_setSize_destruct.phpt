--TEST--
SplFixedArray::setSize in offsetSet destructor (#81429)
--FILE--
<?php
$values = new SplFixedArray(1);
$values->offsetSet(0, new HasDestructor());
$values->offsetSet(0, false);
echo "Done\n";

class HasDestructor {
    public function __destruct() {
        global $values;
        var_dump($values);
        $values->setSize($values->getSize() - 1);
        var_dump($values);
    }
}

$values->setSize(5);
$values->offsetSet(4, new HasDestructor());
echo "Done\n";
--EXPECT--
object(SplFixedArray)#1 (1) {
  [0]=>
  bool(false)
}
object(SplFixedArray)#1 (1) {
  [0]=>
  bool(false)
}
Done
Done
object(SplFixedArray)#1 (5) {
  [0]=>
  NULL
  [1]=>
  NULL
  [2]=>
  NULL
  [3]=>
  NULL
  [4]=>
  object(HasDestructor)#2 (0) {
  }
}
object(SplFixedArray)#1 (4) {
  [0]=>
  NULL
  [1]=>
  NULL
  [2]=>
  NULL
  [3]=>
  NULL
}
