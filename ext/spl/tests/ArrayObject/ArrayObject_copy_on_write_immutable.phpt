--TEST--
ArrayObject and ArrayIterator share immutable arrays and separate them before iterating or writing
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.protect_memory=1
opcache.file_update_protection=0
--FILE--
<?php
// Immutable arrays (literals, which opcache stores in shared memory, and the
// empty array) are shared, and separated before being iterated or modified

class Consts {
    const LIST = [1, 2, 3];
    const MAP = ['b' => 2, 'a' => 1, 'c' => [3, 4]];
    const NAT = ['b10' => 'x10', 'b9' => 'x9', 'B1' => 'X1'];
}

function literal() {
    return ['x' => 1, 'y' => 2, 'z' => 3];
}

echo "-- foreach by value with writes\n";
foreach (['ArrayIterator', 'ArrayObject'] as $class) {
    $o = new $class(Consts::LIST);
    foreach ($o as $k => $v) {
        echo "$k=$v ";
        $o[$k] = $v * 10;
        if ($k === 0) {
            $o[] = 4;
        }
    }
    echo json_encode($o->getArrayCopy()), "\n";
}

echo "-- foreach by reference\n";
foreach (['ArrayIterator', 'ArrayObject'] as $class) {
    $o = new $class(literal());
    foreach ($o as $k => &$v) {
        $v = "$k$v";
    }
    unset($v);
    echo json_encode($o->getArrayCopy()), "\n";
}

echo "-- ArrayIterator methods\n";
$it = new ArrayIterator(literal());
var_dump($it->valid(), $it->key(), $it->current());
$it->next();
var_dump($it->key(), $it->current());
$it->seek(2);
var_dump($it->key());
$it->rewind();
var_dump($it->key());
$it['w'] = 0;
$it->next();
$it->next();
$it->next();
var_dump($it->key(), $it->current());
$it = new ArrayIterator(literal());
$it->seek(1);
unset($it['x']);
var_dump($it->key());

echo "-- first call on the iterator\n";
foreach (['valid', 'current', 'key', 'next', 'rewind'] as $method) {
    $it = new ArrayIterator(literal());
    echo $method, ': ', json_encode($it->$method()), ' ', $it->key(), '=', $it->current(), "\n";
    $it = (new ArrayObject(literal()))->getIterator();
    echo $method, ': ', json_encode($it->$method()), ' ', $it->key(), '=', $it->current(), "\n";
}
$it = new ArrayIterator(literal());
$it->seek(1);
echo $it->key(), "\n";
$it = new RecursiveArrayIterator(['a' => [1, 2]]);
var_dump($it->hasChildren());
$it = new RecursiveArrayIterator(['a' => [1, 2]]);
echo json_encode($it->getChildren()->getArrayCopy()), "\n";

echo "-- getIterator()\n";
$ao = new ArrayObject(literal());
$it1 = $ao->getIterator();
$it2 = $ao->getIterator();
$it1->next();
foreach ($it2 as $k => $v) {
    echo "$k=$v ";
    $ao[$k] = -$v;
}
echo $it1->key(), ' ', json_encode($ao->getArrayCopy()), "\n";
$ao = new ArrayObject(literal());
$it = $ao->getIterator();
$it->next();
$ao->exchangeArray(Consts::MAP);
var_dump($it->key());

echo "-- RecursiveArrayIterator\n";
$it = new RecursiveArrayIterator(['a' => [1, [2, 3]], 'b' => 4]);
foreach (new RecursiveIteratorIterator($it, RecursiveIteratorIterator::SELF_FIRST) as $k => $v) {
    echo $k, '=', json_encode($v), ' ';
}
echo "\n";
$it->rewind();
$children = $it->getChildren();
$children[] = 5;
foreach ($children as $v) {
    echo json_encode($v), ' ';
}
echo json_encode($it->getArrayCopy()), "\n";

echo "-- sort\n";
foreach (['asort', 'ksort', 'natsort', 'natcasesort'] as $sort) {
    $ao = new ArrayObject(Consts::NAT);
    $ao->$sort();
    echo json_encode($ao->getArrayCopy()), ' ';
}
$ao = new ArrayObject([]);
$ao->ksort();
$ao[] = 0;
echo json_encode($ao->getArrayCopy()), ' ';
$ao = new ArrayObject(Consts::LIST);
$ao->uasort(fn ($a, $b) => $b <=> $a);
$ao->uksort(fn ($a, $b) => $a <=> $b);
echo json_encode($ao->getArrayCopy()), "\n";

echo "-- copies\n";
$ao = new ArrayObject(Consts::MAP);
$copy = $ao->getArrayCopy();
$clone = clone $ao;
$cast = (array) $ao;
$old = $ao->exchangeArray(Consts::LIST);
$ao[] = 4;
$clone['d'] = 5;
$copy['e'] = 6;
$cast['f'] = 7;
$old['g'] = 8;
echo json_encode([$ao->getArrayCopy(), $clone->getArrayCopy(), $copy, $cast, $old]), "\n";
var_dump($ao->getArrayCopy() === [1, 2, 3, 4], (new ArrayIterator([]))->getArrayCopy() === []);

echo "-- serialization and dumps\n";
$ao = new ArrayObject(Consts::MAP);
$it = new ArrayIterator(Consts::LIST);
echo serialize($ao), "\n", serialize($it), "\n", $ao->serialize(), "\n";
$ao2 = unserialize(serialize($ao));
$ao2['d'] = 1;
echo json_encode($ao2), ' ', json_encode($ao), ' ', count($ao), "\n";
var_export($ao);
echo "\n";
var_dump($it, $ao == new ArrayObject(Consts::MAP), isset($ao['a']), $ao['c'][1]);
print_r($it);
echo "\n";

echo "-- empty array\n";
foreach (['ArrayIterator', 'ArrayObject'] as $class) {
    $o = new $class([]);
    foreach ($o as $v) {
        echo "unreachable\n";
    }
    $it = $o instanceof ArrayObject ? $o->getIterator() : $o;
    $it->rewind();
    var_dump($it->valid(), $it->key(), $it->current(), count($o));
    $o[] = 'a';
    var_dump($it->valid(), $it->current());
    $e = new $class(Consts::LIST);
    $e->__construct([]);
    var_dump(iterator_to_array($e), iterator_count($e));
}
$it = new ArrayIterator([]);
$it->rewind();
$it->next();
$it->next();
$it[] = 'a';
$it[] = 'b';
var_dump($it->key(), $it->current());
$ao = new ArrayObject([]);
$it = $ao->getIterator();
$it->rewind();
$ao[] = 'c';
unset($ao[0]);
$ao[] = 'd';
var_dump($it->key(), $it->current());
$ao = new ArrayObject(Consts::LIST);
$it = $ao->getIterator();
$it->next();
$ao->exchangeArray([]);
var_dump($it->valid());
$ao[] = 'e';
$ao[] = 'f';
var_dump($it->valid(), $it->current());
foreach (new ArrayObject([]) as &$v) {
    echo "unreachable\n";
}
$it = new ArrayIterator([]);
unset($it['x']);
var_dump(count($it));
try {
    $it->seek(0);
} catch (OutOfBoundsException $e) {
    echo $e->getMessage(), "\n";
}
$append = new AppendIterator();
$append->append(new ArrayIterator([]));
$append->append(new ArrayIterator(Consts::LIST));
echo json_encode(iterator_to_array($append, false)), "\n";

echo "-- literals are unchanged\n";
var_dump(Consts::LIST, Consts::MAP, Consts::NAT, literal(), []);
?>
--EXPECT--
-- foreach by value with writes
0=1 1=2 2=3 3=4 [10,20,30,40]
0=1 1=2 2=3 3=4 [10,20,30,40]
-- foreach by reference
{"x":"x1","y":"y2","z":"z3"}
{"x":"x1","y":"y2","z":"z3"}
-- ArrayIterator methods
bool(true)
string(1) "x"
int(1)
string(1) "y"
int(2)
string(1) "z"
string(1) "x"
string(1) "w"
int(0)
string(1) "y"
-- first call on the iterator
valid: true x=1
valid: true x=1
current: 1 x=1
current: 1 x=1
key: "x" x=1
key: "x" x=1
next: null y=2
next: null y=2
rewind: null x=1
rewind: null x=1
y
bool(true)
[1,2]
-- getIterator()
x=1 y=2 z=3 y {"x":-1,"y":-2,"z":-3}
string(1) "a"
-- RecursiveArrayIterator
a=[1,[2,3]] 0=1 1=[2,3] 0=2 1=3 b=4 
1 [2,3] 5 {"a":[1,[2,3]],"b":4}
-- sort
{"B1":"X1","b10":"x10","b9":"x9"} {"B1":"X1","b10":"x10","b9":"x9"} {"B1":"X1","b9":"x9","b10":"x10"} {"B1":"X1","b9":"x9","b10":"x10"} [0] [1,2,3]
-- copies
[[1,2,3,4],{"b":2,"a":1,"c":[3,4],"d":5},{"b":2,"a":1,"c":[3,4],"e":6},{"b":2,"a":1,"c":[3,4],"f":7},{"b":2,"a":1,"c":[3,4],"g":8}]
bool(true)
bool(true)
-- serialization and dumps
O:11:"ArrayObject":4:{i:0;i:0;i:1;a:3:{s:1:"b";i:2;s:1:"a";i:1;s:1:"c";a:2:{i:0;i:3;i:1;i:4;}}i:2;a:0:{}i:3;N;}
O:13:"ArrayIterator":4:{i:0;i:0;i:1;a:3:{i:0;i:1;i:1;i:2;i:2;i:3;}i:2;a:0:{}i:3;N;}
x:i:0;a:3:{s:1:"b";i:2;s:1:"a";i:1;s:1:"c";a:2:{i:0;i:3;i:1;i:4;}};m:a:0:{}
{"b":2,"a":1,"c":[3,4],"d":1} {"b":2,"a":1,"c":[3,4]} 3
\ArrayObject::__set_state(array(
   'b' => 2,
   'a' => 1,
   'c' => 
  array (
    0 => 3,
    1 => 4,
  ),
))
object(ArrayIterator)#7 (1) {
  ["storage":"ArrayIterator":private]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}
bool(true)
bool(true)
int(4)
ArrayIterator Object
(
    [storage:ArrayIterator:private] => Array
        (
            [0] => 1
            [1] => 2
            [2] => 3
        )

)

-- empty array
bool(false)
NULL
NULL
int(0)
bool(true)
string(1) "a"
array(0) {
}
int(0)
bool(false)
NULL
NULL
int(0)
bool(true)
string(1) "a"
array(0) {
}
int(0)
int(0)
string(1) "a"
int(1)
string(1) "d"
bool(false)
bool(true)
string(1) "f"
int(0)
Seek position 0 is out of range
[1,2,3]
-- literals are unchanged
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(3) {
  ["b"]=>
  int(2)
  ["a"]=>
  int(1)
  ["c"]=>
  array(2) {
    [0]=>
    int(3)
    [1]=>
    int(4)
  }
}
array(3) {
  ["b10"]=>
  string(3) "x10"
  ["b9"]=>
  string(2) "x9"
  ["B1"]=>
  string(2) "X1"
}
array(3) {
  ["x"]=>
  int(1)
  ["y"]=>
  int(2)
  ["z"]=>
  int(3)
}
array(0) {
}
