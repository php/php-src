--TEST--
unserialize() with accessors
--FILE--
<?php

class Test {
    public $prop3 {
        get { echo __METHOD__, "\n"; }
        set { echo __METHOD__, "\n"; }
    }

    public function __construct(
        public $prop1 { get; set; },
        public $prop2 { get; },
    ) {}
}

$test = new Test(1, 2);
var_dump($s = serialize($test));
var_dump($test2 = unserialize($s));
$test2->prop3;
$test2->prop3 = 42;

// TODO: This should probably be forbidden.
$s = 'O:4:"Test":1:{s:5:"prop3";i:42;}';
var_dump($test3 = unserialize($s));
$test3->prop3;
$test3->prop3 = 42;

?>
--EXPECT--
string(47) "O:4:"Test":2:{s:5:"prop1";i:1;s:5:"prop2";i:2;}"
object(Test)#2 (2) {
  ["prop1"]=>
  int(1)
  ["prop2"]=>
  int(2)
}
Test::$prop3::get
Test::$prop3::set
object(Test)#3 (1) {
  ["prop3"]=>
  int(42)
}
Test::$prop3::get
Test::$prop3::set
