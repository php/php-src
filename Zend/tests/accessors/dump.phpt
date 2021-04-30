--TEST--
Dumping object with accessors
--FILE--
<?php

// The dump always only shows the contents of backing properties,
// not any computed properties.
class Test {
    public $prop = 1;
    public $prop2 = 2 { get; set; }
    public $prop3 {
        get { return 42; }
    }
}

var_dump(new Test);

?>
--EXPECT--
object(Test)#1 (2) {
  ["prop"]=>
  int(1)
  ["prop2"]=>
  int(2)
}
