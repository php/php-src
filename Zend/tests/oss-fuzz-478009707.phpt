--TEST--
OSS-Fuzz #478009707: Assign-op/inc/dec on untyped hooked property backing value
--FILE--
<?php

class C {
    public $prop {
        set {
            $this->prop = $value;
            $this->prop += 1;
            $this->prop++;
            ++$this->prop;
        }
    }
}

$c = new C(1);
$c->prop = 1;
var_dump($c->prop);

$c->prop = PHP_INT_MAX;
var_dump($c->prop);

?>
--EXPECTF--
int(4)
float(%s)
