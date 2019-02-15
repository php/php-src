--TEST--
ZE2 object cloning, 4
--FILE--
<?php
abstract class base {
  public $a = 'base';

  // disallow cloning
  private function __clone() {}
}

class test extends base {
  public $b = 'test';

  // re-enable cloning
  public function __clone() {}

  public function show() {
	var_dump($this);
  }
}

echo "Original\n";
$o1 = new test;
$o1->a = array(1,2);
$o1->b = array(3,4);
$o1->show();

echo "Clone\n";
$o2 = clone $o1;
$o2->show();

echo "Modify\n";
$o2->a = 5;
$o2->b = 6;
$o2->show();

echo "Done\n";
?>
--EXPECT--
Original
object(test)#1 (2) {
  ["b"]=>
  array(2) {
    [0]=>
    int(3)
    [1]=>
    int(4)
  }
  ["a"]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}
Clone
object(test)#2 (2) {
  ["b"]=>
  array(2) {
    [0]=>
    int(3)
    [1]=>
    int(4)
  }
  ["a"]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}
Modify
object(test)#2 (2) {
  ["b"]=>
  int(6)
  ["a"]=>
  int(5)
}
Done
