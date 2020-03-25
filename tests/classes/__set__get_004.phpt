--TEST--
ZE2 __set() and __get()
--FILE--
<?php
class Test {
    protected $x;

    function __get($name) {
        if (isset($this->x[$name])) {
            return $this->x[$name];
        }
        else
        {
            return NULL;
        }
    }

    function __set($name, $val) {
        $this->x[$name] = $val;
    }
}

$foo = new Test();
$bar = new Test();
$bar->baz = "Check";

$foo->bar = $bar;

var_dump($bar->baz);
var_dump($foo->bar->baz);

?>
--EXPECT--
string(5) "Check"
string(5) "Check"
