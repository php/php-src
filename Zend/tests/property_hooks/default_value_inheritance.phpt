--TEST--
Property default values are not inherited
--FILE--
<?php

class P {
    public $a = 1;
    public int $b = 1;
    public $c = 1;
    public int $d = 1;
}

class C extends P {
    public $a { get => parent::$a::get(); }
    public int $b { get => parent::$b::get(); }
    public $c = 2 { get => parent::$c::get(); }
    public int $d = 2 { get => parent::$d::get(); }
}

class GC extends C {
    public $a { get => parent::$a::get(); }
    public int $b { get => parent::$b::get(); }
    public $c { get => parent::$c::get(); }
    public int $d { get => parent::$d::get(); }
}

function test(P $p) {
    var_dump($p->a);
    try {
        var_dump($p->b);
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
    var_dump($p->c);
    try {
        var_dump($p->d);
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}

test(new C);
test(new GC);

?>
--EXPECT--
NULL
Typed property C::$b must not be accessed before initialization
int(2)
int(2)
NULL
Typed property GC::$b must not be accessed before initialization
NULL
Typed property GC::$d must not be accessed before initialization
