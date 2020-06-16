--TEST--
Referencing an uninitialized typed property in __sleep() should be skipped
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
var_dump(serialize($t));
var_dump(unserialize(serialize($t)) == $t);

$t->x = 1;
var_dump(unserialize(serialize($t)) == $t);

$t->y = 2;
var_dump(unserialize(serialize($t)) == $t);

$t->z = 3;
var_dump(unserialize(serialize($t)) == $t);

var_dump($t);
?>
--EXPECT--
string(15) "O:4:"Test":0:{}"
bool(true)
bool(true)
bool(true)
bool(true)
object(Test)#1 (3) {
  ["x"]=>
  int(1)
  ["y":protected]=>
  int(2)
  ["z":"Test":private]=>
  int(3)
}
