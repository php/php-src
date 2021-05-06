--TEST--
ArrayObject/ArrayIterator : serialization
--FILE--
<?php
$o = new ArrayObject();
$y = new StdClass;
$o->append($y);
$o->append($y);
$o->append($o);

var_dump($o[0] === $o[1]);
var_dump($o[2] === $o);

$s1 = serialize($o);
$s2 = $o->serialize();
var_dump($s1);
var_dump($s2);

$o1 = unserialize($s1);

var_dump($o1[0] === $o1[1]);
var_dump($o1[2] === $o1);

$o2 = new ArrayObject();
$o2->unserialize($s2);

var_dump($o2[0] === $o2[1]);
var_dump($o2[2] !== $o2);
var_dump($o2[2][2] === $o2[2]);

echo "#### Extending ArrayObject\n";
unset($o,$x,$s1,$s2,$o1,$o2);
class ArrayObject2 extends ArrayObject {
    public function __serialize(): array {
        return parent::__serialize();
    }

    public function __unserialize($s): void {
        parent::__unserialize($s);
    }
}

$o = new ArrayObject2();
$y = new StdClass;
$o->append($y);
$o->append($y);
$o->append($o);

var_dump($o[0] === $o[1]);
var_dump($o[2] === $o);

$s1 = serialize($o);
$s2 = $o->__serialize();
var_dump($s1);
var_dump($s2);

$o1 = unserialize($s1);

var_dump($o1[0] === $o1[1]);
var_dump($o1[2] === $o1);

$o2 = new ArrayObject2();
$o2->__unserialize($s2);

var_dump($o2[0] === $o2[1]);
var_dump($o2[2] !== $o2);
var_dump($o2[2][2] === $o2[2]);
?>
--EXPECT--
bool(true)
bool(true)
string(96) "O:11:"ArrayObject":4:{i:0;i:0;i:1;a:3:{i:0;O:8:"stdClass":0:{}i:1;r:4;i:2;r:1;}i:2;a:0:{}i:3;N;}"
string(137) "x:i:0;a:3:{i:0;O:8:"stdClass":0:{}i:1;r:3;i:2;O:11:"ArrayObject":4:{i:0;i:0;i:1;a:3:{i:0;r:3;i:1;r:3;i:2;r:5;}i:2;a:0:{}i:3;N;}};m:a:0:{}"
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
#### Extending ArrayObject
bool(true)
bool(true)
string(97) "O:12:"ArrayObject2":4:{i:0;i:0;i:1;a:3:{i:0;O:8:"stdClass":0:{}i:1;r:4;i:2;r:1;}i:2;a:0:{}i:3;N;}"
array(4) {
  [0]=>
  int(0)
  [1]=>
  array(3) {
    [0]=>
    object(stdClass)#8 (0) {
    }
    [1]=>
    object(stdClass)#8 (0) {
    }
    [2]=>
    object(ArrayObject2)#5 (1) {
      ["storage":"ArrayObject":private]=>
      *RECURSION*
    }
  }
  [2]=>
  array(0) {
  }
  [3]=>
  NULL
}
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
