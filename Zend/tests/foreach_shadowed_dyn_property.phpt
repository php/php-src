--TEST--
Dynamic property shadowed by private property
--FILE--
<?php

class Test {
    private $prop = "Test";

    function run() {
        foreach ($this as $k => $v) {
            echo "$k => $v\n";
        }
        var_dump(get_object_vars($this));
    }
}
class Test2 extends Test {
}

$test2 = new Test2;
$test2->prop = "Test2";
$test2->run();

?>
--EXPECT--
prop => Test
array(1) {
  ["prop"]=>
  string(4) "Test"
}
