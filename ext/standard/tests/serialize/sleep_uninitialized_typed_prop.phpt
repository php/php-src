--TEST--
Referencing an uninitialized typed property in __sleep() should result in Error
--FILE--
<?php

class Test {
    public int $x;
    protected int $y;
    private int $z;

    public function __sleep() {
        return ['x', 'y', 'z'];
    }

    public function __set($name, $val) {
        $this->$name = $val;
    }
}

$t = new Test;
try {
    serialize($t);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$t->x = 1;
try {
    serialize($t);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$t->y = 2;
try {
    serialize($t);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$t->z = 3;
try {
    var_dump(unserialize(serialize($t)));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Typed property Test::$x must not be accessed before initialization (in __sleep)
Typed property Test::$y must not be accessed before initialization (in __sleep)
Typed property Test::$z must not be accessed before initialization (in __sleep)
object(Test)#3 (3) {
  ["x"]=>
  int(1)
  ["y":protected]=>
  int(2)
  ["z":"Test":private]=>
  int(3)
}
