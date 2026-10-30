--TEST--
Avoid spaces between parent hook call nodes
--FILE--
<?php

class A {
    public int $prop {
        get {
            return 41;
        }
    }
}

class B extends A {
    public int $prop {
        get {
            var_dump(parent::$prop::get());
            var_dump(parent :: $prop :: get());
            return 42;
        }
    }
}

class C {
    public static $prop = 'E';
}

class D extends C {
    public static function test() {
        return (parent::$prop)::get();
    }
}

class E {
    public static function get() {
        return 43;
    }
}

$b = new B;
var_dump($b->prop);
var_dump(D::test());

?>
--EXPECT--
int(41)
int(41)
int(42)
int(43)
