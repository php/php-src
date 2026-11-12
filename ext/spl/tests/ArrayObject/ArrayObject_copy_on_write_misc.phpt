--TEST--
ArrayObject and ArrayIterator sharing an array: serialization and dumps
--FILE--
<?php
echo "-- serialize\n";
$source = ['a' => 1, 'b' => [2]];
$ao = new ArrayObject($source);
$it = new ArrayIterator($source);
echo serialize([$source, $ao, $it]), "\n";
[$s, $ao2, $it2] = unserialize(serialize([$source, $ao, $it]));
$ao2['a'] = 2;
$it2['b'][] = 3;
var_dump($s, $ao2->getArrayCopy(), $it2->getArrayCopy());

echo "-- var_dump, var_export, json_encode, count, ==\n";
$ao = new ArrayObject($source);
var_dump($ao);
var_export($ao); echo "\n";
echo json_encode($ao), "\n";
var_dump(count($ao), $ao == new ArrayObject($source), $ao == new ArrayObject([]));
?>
--EXPECT--
-- serialize
a:3:{i:0;a:2:{s:1:"a";i:1;s:1:"b";a:1:{i:0;i:2;}}i:1;O:11:"ArrayObject":4:{i:0;i:0;i:1;a:2:{s:1:"a";i:1;s:1:"b";a:1:{i:0;i:2;}}i:2;a:0:{}i:3;N;}i:2;O:13:"ArrayIterator":4:{i:0;i:0;i:1;a:2:{s:1:"a";i:1;s:1:"b";a:1:{i:0;i:2;}}i:2;a:0:{}i:3;N;}}
array(2) {
  ["a"]=>
  int(1)
  ["b"]=>
  array(1) {
    [0]=>
    int(2)
  }
}
array(2) {
  ["a"]=>
  int(2)
  ["b"]=>
  array(1) {
    [0]=>
    int(2)
  }
}
array(2) {
  ["a"]=>
  int(1)
  ["b"]=>
  array(2) {
    [0]=>
    int(2)
    [1]=>
    int(3)
  }
}
-- var_dump, var_export, json_encode, count, ==
object(ArrayObject)#5 (1) {
  ["storage":"ArrayObject":private]=>
  array(2) {
    ["a"]=>
    int(1)
    ["b"]=>
    array(1) {
      [0]=>
      int(2)
    }
  }
}
\ArrayObject::__set_state(array(
   'a' => 1,
   'b' => 
  array (
    0 => 2,
  ),
))
{"a":1,"b":[2]}
int(2)
bool(true)
bool(false)
