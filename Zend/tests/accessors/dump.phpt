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

$test = new Test;
var_dump($test);
var_dump(get_object_vars($test));
var_dump((array) $test);

foreach ($test as $prop => $value) {
    echo "$prop => $value\n";
}

?>
--EXPECT--
object(Test)#1 (2) {
  ["prop"]=>
  int(1)
  ["prop2"]=>
  int(2)
}
array(2) {
  ["prop"]=>
  int(1)
  ["prop2"]=>
  int(2)
}
array(2) {
  ["prop"]=>
  int(1)
  ["prop2"]=>
  int(2)
}
prop => 1
prop2 => 2
