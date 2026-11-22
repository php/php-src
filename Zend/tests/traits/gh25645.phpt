--TEST--
GH-25645: Segfault when a trait constant or property using parent is compared during linking
--CREDITS--
rlerdorf
--FILE--
<?php

class P {
    const Y = 1;
}

trait T1 {
    const X = parent::class;
    const Y2 = parent::Y;
    const Z = [parent::class . '!'];
    public $x = parent::class;
    public static $y = parent::Y;
}

trait T2 {
    const X = parent::class;
    const Y2 = parent::Y;
    public $x = parent::class;
    public static $y = parent::Y;
}

trait T3 {
    const W = 1;
}

class C extends P {
    use T1, T2, T3;
    const X = 'P';
    const Z = ['P!'];
    const W = parent::Y;
}

var_dump(C::X, C::Y2, C::Z, C::W, (new C)->x, C::$y);

?>
--EXPECT--
string(1) "P"
int(1)
array(1) {
  [0]=>
  string(2) "P!"
}
int(1)
string(1) "P"
int(1)
