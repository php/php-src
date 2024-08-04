--TEST--
Property access within closure
--FILE--
<?php

class A {
    public $prop {
        get {
            return function () {
                return $this->prop;
            };
        }
    }
}

$a = new A();
$c = $a->prop;
var_dump(get_class($c));
$d = $c();
var_dump(get_class($d));

?>
--EXPECT--
string(7) "Closure"
string(7) "Closure"
