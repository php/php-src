--TEST--
Bug #21669 ("$obj = new $this->var;" doesn't work)
--FILE--
<?php
class Test {
    function say_hello() {
        echo "Hello world";
    }
}

class Factory {
    public $name = "Test";
    function create() {
        $obj = new $this->name; /* Parse error */
        return $obj;
    }
}
$factory = new Factory;
$test = $factory->create();
$test->say_hello();
?>
--EXPECT--
Hello world
