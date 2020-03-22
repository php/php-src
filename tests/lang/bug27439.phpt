--TEST--
Bug #27439 (foreach() with $this segfaults)
--FILE--
<?php

class test_props {
    public $a = 1;
    public $b = 2;
    public $c = 3;
}

class test {
    public $array = array(1,2,3);
    public $string = "string";

    public function __construct() {
        $this->object = new test_props;
    }

    public function getArray() {
        return $this->array;
    }

    public function getString() {
        return $this->string;
    }

    public function case1() {
        foreach ($this->array as $foo) {
            echo $foo;
        }
    }

    public function case2() {
        foreach ($this->foobar as $foo);
    }

    public function case3() {
        foreach ($this->string as $foo);
    }

    public function case4() {
        foreach ($this->getArray() as $foo);
    }

    public function case5() {
        foreach ($this->getString() as $foo);
    }

    public function case6() {
        foreach ($this->object as $foo) {
            echo $foo;
        }
    }
}
$test = new test();
$test->case1();
$test->case2();
$test->case3();
$test->case4();
$test->case5();
$test->case6();
echo "\n";
echo "===DONE===";
?>
--EXPECTF--
123
Warning: Undefined property: test::$foobar in %s on line %d

Warning: foreach() argument must be of type array|object, null given in %s on line %d

Warning: foreach() argument must be of type array|object, string given in %s on line %d

Warning: foreach() argument must be of type array|object, string given in %s on line %d
123
===DONE===
