--TEST--
Dumping object with accessors
--FILE--
<?php

// The dump always only shows the contents of backing properties,
// not any computed properties.
// TODO: prop5 shouldn't have a backing property.
class Test {
    public $prop = 1;
    public $prop2 = 2 { get; set; }
    public $prop3 = 3 {
        get;
        set {}
    }
    public $prop4 = 4 {
        get { return $this->prop4 * 2; }
        set;
    }
    public $prop5 {
        get { return 42; }
    }
}

var_dump(new Test);

?>
--EXPECT--
object(Test)#1 (4) {
  ["prop"]=>
  int(1)
  ["prop2"]=>
  int(2)
  ["prop3"]=>
  int(3)
  ["prop4"]=>
  int(4)
}
