--TEST--
GH-18845: Use-after-free of object through __isset() and globals
--FILE--
<?php

class C {
    public function __isset($x) {
        $GLOBALS['c'] = null;
        return true;
    }
}

$c = new C;
var_dump($c->prop ?? 1);

?>
--EXPECT--
int(1)
