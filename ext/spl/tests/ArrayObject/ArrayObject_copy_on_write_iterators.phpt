--TEST--
ArrayObject and ArrayIterator keep their iteration position when separating a shared array
--FILE--
<?php
// Iteration keeps its position when the storage is separated from the array it
// was built from, including when that array has holes that a copy compacts

function source() {
    $a = ['z' => 0, 'a' => 1, 'b' => 2, 'c' => 3, 'd' => 4];
    unset($a['z']);
    return $a;
}

echo "-- write during foreach\n";
$source = source();
$it = new ArrayIterator($source);
foreach ($it as $k => $v) {
    echo "$k=$v ";
    $it[$k] = $v * 10;
}
echo "\n", json_encode($it->getArrayCopy()), ' ', json_encode($source), "\n";

echo "-- write during manual iteration\n";
$source = source();
$it = new ArrayIterator($source);
$it->rewind();
$it->next();
$it['e'] = 5;
for (; $it->valid(); $it->next()) {
    echo $it->key(), '=', $it->current(), ' ';
}
echo "\n";

echo "-- unset during foreach\n";
$source = source();
$it = new ArrayIterator($source);
foreach ($it as $k => $v) {
    echo "$k=$v ";
    if ($k === 'b') {
        unset($it['c']);
    }
}
echo "\n", json_encode($it->getArrayCopy()), "\n";

echo "-- ArrayObject iterator, write through the ArrayObject\n";
$source = source();
$ao = new ArrayObject($source);
foreach ($ao as $k => $v) {
    echo "$k=$v ";
    $ao[$k] = -$v;
}
echo "\n", json_encode($ao->getArrayCopy()), ' ', json_encode($source), "\n";

echo "-- two ArrayObject iterators\n";
$source = source();
$ao = new ArrayObject($source);
$it1 = $ao->getIterator();
$it2 = $ao->getIterator();
$it1->next();
$it2->next();
$it2->next();
$ao['f'] = 6;
echo $it1->key(), ' ', $it2->key(), "\n";
$it1->next();
$it2->next();
echo $it1->key(), ' ', $it2->key(), "\n";

echo "-- source modified during foreach\n";
$source = source();
$it = new ArrayIterator($source);
foreach ($it as $k => $v) {
    echo "$k=$v ";
    unset($source['c']);
    $source['g'] = 7;
}
echo "\n", json_encode($it->getArrayCopy()), ' ', json_encode($source), "\n";

echo "-- foreach by reference\n";
$source = source();
$ao = new ArrayObject($source);
foreach ($ao as $k => &$v) {
    echo "$k=$v ";
    $v *= 2;
}
unset($v);
echo "\n", json_encode($ao->getArrayCopy()), ' ', json_encode($source), "\n";

echo "-- nested foreach with writes\n";
$source = source();
$ao = new ArrayObject($source);
foreach ($ao as $k1 => $v1) {
    foreach ($ao as $k2 => $v2) {
        if ($k2 === 'b') {
            $ao['b'] = $v2 + 1;
        }
    }
    echo "$k1=$v1 ";
}
echo "\n", json_encode($ao->getArrayCopy()), "\n";

echo "-- getArrayCopy() then write during foreach\n";
$it = new ArrayIterator(source());
foreach ($it as $k => $v) {
    $copy = $it->getArrayCopy();
    $it[$k] = $v + 100;
    echo "$k=$v ";
}
echo "\n", json_encode($it->getArrayCopy()), ' ', json_encode($copy), "\n";

echo "-- sort during foreach\n";
$source = [3 => 'c', 1 => 'a', 2 => 'b'];
$ao = new ArrayObject($source);
foreach ($ao as $k => $v) {
    echo "$k=$v ";
    if ($k === 3) {
        $ao->ksort();
    }
}
echo "\n", json_encode($ao->getArrayCopy()), ' ', json_encode($source), "\n";

echo "-- RecursiveArrayIterator\n";
$source = ['a' => [1, 2], 'b' => ['c' => [3]]];
$it = new RecursiveArrayIterator($source);
$children = $it->getChildren();
$children[] = 'x';
foreach (new RecursiveIteratorIterator($it) as $k => $v) {
    echo "$k=$v ";
}
echo "\n", json_encode($children->getArrayCopy()), ' ', json_encode($source), "\n";
?>
--EXPECT--
-- write during foreach
a=1 b=2 c=3 d=4 
{"a":10,"b":20,"c":30,"d":40} {"a":1,"b":2,"c":3,"d":4}
-- write during manual iteration
b=2 c=3 d=4 e=5 
-- unset during foreach
a=1 b=2 d=4 
{"a":1,"b":2,"d":4}
-- ArrayObject iterator, write through the ArrayObject
a=1 b=2 c=3 d=4 
{"a":-1,"b":-2,"c":-3,"d":-4} {"a":1,"b":2,"c":3,"d":4}
-- two ArrayObject iterators
b c
c d
-- source modified during foreach
a=1 b=2 c=3 d=4 
{"a":1,"b":2,"c":3,"d":4} {"a":1,"b":2,"d":4,"g":7}
-- foreach by reference
a=1 b=2 c=3 d=4 
{"a":2,"b":4,"c":6,"d":8} {"a":1,"b":2,"c":3,"d":4}
-- nested foreach with writes
a=1 b=3 c=3 d=4 
{"a":1,"b":6,"c":3,"d":4}
-- getArrayCopy() then write during foreach
a=1 b=2 c=3 d=4 
{"a":101,"b":102,"c":103,"d":104} {"a":101,"b":102,"c":103,"d":4}
-- sort during foreach
3=c 2=b 3=c 
{"1":"a","2":"b","3":"c"} {"3":"c","1":"a","2":"b"}
-- RecursiveArrayIterator
0=1 1=2 0=3 
[1,2,"x"] {"a":[1,2],"b":{"c":[3]}}
