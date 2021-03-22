--TEST--
Backing property
--FILE--
<?php

class Test {
    public int $prop {
        get { return $this->prop * 2; }
        set { $this->prop = $value * 2; }
    }
}

$test = new Test;
$test->prop = 10;
var_dump($test->prop);
var_dump($test);

?>
--EXPECT--
int(40)
object(Test)#1 (1) {
  ["prop"]=>
  int(20)
}
