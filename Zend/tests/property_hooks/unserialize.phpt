--TEST--
unserialize() with hooks
--FILE--
<?php

class Test {
    public $prop2 {
        get { echo __METHOD__, "\n"; }
    }

    public $prop3 {
        get { echo __METHOD__, "\n"; }
        set { echo __METHOD__, "\n"; }
    }

    public function __construct(
        public $prop1,
    ) {}
}

$test = new Test(1, 2);
var_dump($s = serialize($test));
var_dump($test_u = unserialize($s));
$test_u->prop3;
$test_u->prop3 = 42;

$s = 'O:4:"Test":1:{s:5:"prop3";i:42;}';
var_dump(unserialize($s));
echo "\n";

// Override implicit hook with explicit.
class Test2 extends Test {
    public $prop1 {
        get { echo __METHOD__, "\n"; }
    }
}

$test2 = new Test2(1, 2);
var_dump($s = serialize($test2));
var_dump($test2_u = unserialize($s));
$test2_u->prop1;
$test2_u->prop1 = 42;

$s = 'O:5:"Test2":1:{s:5:"prop1";i:42;}';
var_dump(unserialize($s));

?>
--EXPECTF--
string(31) "O:4:"Test":1:{s:5:"prop1";i:1;}"
object(Test)#2 (1) {
  ["prop1"]=>
  int(1)
}
Test::$prop3::get
Test::$prop3::set

Warning: unserialize(): Cannot unserialize value for virtual property Test::$prop3 in %s on line %d

Warning: unserialize(): Error at offset 26 of 32 bytes in %s on line %d
bool(false)

string(32) "O:5:"Test2":1:{s:5:"prop1";i:1;}"
object(Test2)#4 (1) {
  ["prop1"]=>
  int(1)
}
Test2::$prop1::get
object(Test2)#5 (1) {
  ["prop1"]=>
  int(42)
}
