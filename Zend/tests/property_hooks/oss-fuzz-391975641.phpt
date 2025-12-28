--TEST--
OSS-Fuzz #391975641: Segfault when creating reference from backing value
--FILE--
<?php

class C {
    public $prop {
        get => $this->prop;
        set {
            $this->prop = &$value;
            $value = &$this->prop;
        }
    }
}

$c = new C;
$c->prop = 1;
var_dump($c->prop);

?>
--EXPECT--
int(1)
