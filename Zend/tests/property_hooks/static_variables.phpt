--TEST--
Hooks containing static variables
--FILE--
<?php

class Test {
    public $prop {
        get {
            static $foo = [];
            static $count = 1;
            $foo[] = $count++;
            return $foo;
        }
    }
}

$test = new Test;
var_dump($test->prop);
var_dump($test->prop);
var_dump($test->prop);

?>
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
