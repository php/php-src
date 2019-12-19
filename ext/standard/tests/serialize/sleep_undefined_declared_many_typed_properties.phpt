--TEST--
__sleep() returning undefined declared typed properties
--FILE--
<?php

class Test {
    public int $a;
    public int $b;
    public int $c;
    public int $d;
    public int $e;
    public ?int $f;
    public int $g;
    public iterable $h;
    public ?ArrayObject $i;
    public stdClass $j;

    public function __construct() {
        unset($this->a,$this->b,$this->c,$this->d,$this->e,$this->f,$this->g,$this->h,$this->i,$this->j);
    }

    public function __sleep() {
        return ['a','b','c','d','e','f','g','h','i','j'];
    }
}

var_dump($s = serialize(new Test));
var_dump(unserialize($s));
$t = new Test();
$t->a = 1234;
$t->f = null;
var_dump($s = serialize($t));
var_dump(unserialize($s));

?>
--EXPECTF--
Notice: serialize(): "a" returned as member variable from __sleep() but does not exist in %s on line %d

Notice: serialize(): "b" returned as member variable from __sleep() but does not exist in %s on line %d

Notice: serialize(): "c" returned as member variable from __sleep() but does not exist in %s on line %d

Notice: serialize(): "d" returned as member variable from __sleep() but does not exist in %s on line %d

Notice: serialize(): "e" returned as member variable from __sleep() but does not exist in %s on line %d

Notice: serialize(): "f" returned as member variable from __sleep() but does not exist in %s on line %d

Notice: serialize(): "g" returned as member variable from __sleep() but does not exist in %s on line %d

Notice: serialize(): "h" returned as member variable from __sleep() but does not exist in %s on line %d

Notice: serialize(): "i" returned as member variable from __sleep() but does not exist in %s on line %d

Notice: serialize(): "j" returned as member variable from __sleep() but does not exist in %s on line %d
string(16) "O:4:"Test":00:{}"
object(Test)#1 (0) {
  ["a"]=>
  uninitialized(int)
  ["b"]=>
  uninitialized(int)
  ["c"]=>
  uninitialized(int)
  ["d"]=>
  uninitialized(int)
  ["e"]=>
  uninitialized(int)
  ["f"]=>
  uninitialized(?int)
  ["g"]=>
  uninitialized(int)
  ["h"]=>
  uninitialized(iterable)
  ["i"]=>
  uninitialized(?ArrayObject)
  ["j"]=>
  uninitialized(stdClass)
}

Notice: serialize(): "b" returned as member variable from __sleep() but does not exist in %s on line %d

Notice: serialize(): "c" returned as member variable from __sleep() but does not exist in %s on line %d

Notice: serialize(): "d" returned as member variable from __sleep() but does not exist in %s on line %d

Notice: serialize(): "e" returned as member variable from __sleep() but does not exist in %s on line %d

Notice: serialize(): "g" returned as member variable from __sleep() but does not exist in %s on line %d

Notice: serialize(): "h" returned as member variable from __sleep() but does not exist in %s on line %d

Notice: serialize(): "i" returned as member variable from __sleep() but does not exist in %s on line %d

Notice: serialize(): "j" returned as member variable from __sleep() but does not exist in %s on line %d
string(41) "O:4:"Test":02:{s:1:"a";i:1234;s:1:"f";N;}"
object(Test)#2 (2) {
  ["a"]=>
  int(1234)
  ["b"]=>
  uninitialized(int)
  ["c"]=>
  uninitialized(int)
  ["d"]=>
  uninitialized(int)
  ["e"]=>
  uninitialized(int)
  ["f"]=>
  NULL
  ["g"]=>
  uninitialized(int)
  ["h"]=>
  uninitialized(iterable)
  ["i"]=>
  uninitialized(?ArrayObject)
  ["j"]=>
  uninitialized(stdClass)
}
