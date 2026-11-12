--TEST--
ArrayObject and ArrayIterator copy arrays holding references
--FILE--
<?php
// Arrays holding references are still copied, as zend_array_dup() unwraps
// references with refcount 1

echo "-- dangling references in the source\n";
$source = ['a' => null, 'b' => 2];
foreach ($source as &$v);
unset($v);
foreach ([new ArrayIterator($source), new ArrayObject($source)] as $object) {
    var_dump(isset($object['a']));
    $array = iterator_to_array($object);
    $array['b'] = 3;
    $cache = new CachingIterator(new IteratorIterator($object), CachingIterator::FULL_CACHE);
    foreach ($cache as $v);
    $source['b'] = 4;
    var_dump($object['b'], $source['b']);
    $source['b'] = 2;
}

echo "-- shared references in the source\n";
$x = 1;
$source = ['x' => &$x];
$it = new ArrayIterator($source);
$x = 2;
var_dump($it['x']);
unset($x);
$source['x'] = 3;
var_dump($it['x']);

echo "-- dangling reference in the storage\n";
$ao = new ArrayObject(['x' => 1]);
$r = &$ao['x'];
unset($r);
$copy = $ao->getArrayCopy();
$array = iterator_to_array($ao);
$copy['x'] = 2;
$clone = clone $ao;
$array['x'] = 3;
var_dump($copy['x'], $clone['x'], $ao['x']);

echo "-- built inside foreach by reference over the source\n";
$source = [1, 2, 3];
foreach ($source as $k => &$v) {
    if ($k === 0) {
        $it = new ArrayIterator($source);
    }
    $v *= 10;
}
unset($v);
var_dump($it->getArrayCopy());
?>
--EXPECT--
-- dangling references in the source
bool(false)
int(2)
int(4)
bool(false)
int(2)
int(4)
-- shared references in the source
int(2)
int(3)
-- dangling reference in the storage
int(2)
int(3)
int(3)
-- built inside foreach by reference over the source
array(3) {
  [0]=>
  &int(10)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
