--TEST--
Parent property hook call can not be partially applied
--FILE--
<?php

class C {
    public $a {
        set($value) { var_dump($value); }
    }
}

class D extends C {
    public $a {
        set($value) {
            $f = parent::$a::set(?);
        }
    }
}

$d = new D();
$d->a = 1;

?>
--EXPECTF--
Fatal error: Cannot create Closure for parent property hook call in %s on line %d
