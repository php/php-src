--TEST--
GH-16040: Use-after-free on object released in hook
--FILE--
<?php

class A {
    public $bar {
        get {
            $GLOBALS['a'] = null;
            return 42;
        }
    }
}

$a = new A();
var_dump($a->bar);

?>
--EXPECT--
int(42)
