--TEST--
Bug #73288 (Segfault in __clone > Exception.toString > __get)
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php

class NoClone {
    public function __clone() {
        throw new Exception("No Cloneable");
    }
}

class C {
    public function __get($name) {
        return new NoClone;
    }
}

function test_clone() {
    $c = new C;
    $b = clone $c->x;
}

test_clone();
?>
--EXPECTF--
Fatal error: Uncaught Exception: No Cloneable in %sbug73288.php:%d
Stack trace:
#0 %s(%d): NoClone->__clone()
#1 %s(%d): test_clone()
#2 {main}
  thrown in %sbug73288.php on line %d
