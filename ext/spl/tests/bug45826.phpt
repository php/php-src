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
var_dump($s1);

$o1 = unserialize($s1);

var_dump($o1[0] === $o1[1]);
var_dump($o1[2] === $o1);

echo "#### Extending ArrayObject\n";
unset($o,$x,$s1,$s2,$o1,$o2);
class ArrayObject2 extends ArrayObject {
    public function __serialize() {
        return parent::__serialize();
    }

    public function __unserialize($s) {
        return parent::__unserialize($s);
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
var_dump($s1);

$o1 = unserialize($s1);

var_dump($o1[0] === $o1[1]);
var_dump($o1[2] === $o1);
?>
--EXPECT--
bool(true)
bool(true)
string(90) "O:11:"ArrayObject":3:{i:0;i:0;i:1;a:3:{i:0;O:8:"stdClass":0:{}i:1;r:4;i:2;r:1;}i:2;a:0:{}}"
bool(true)
bool(true)
#### Extending ArrayObject
bool(true)
bool(true)
string(91) "O:12:"ArrayObject2":3:{i:0;i:0;i:1;a:3:{i:0;O:8:"stdClass":0:{}i:1;r:4;i:2;r:1;}i:2;a:0:{}}"
bool(true)
bool(true)
