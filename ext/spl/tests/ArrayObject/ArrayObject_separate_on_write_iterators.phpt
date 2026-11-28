--TEST--
ArrayObject and ArrayIterator keep their iteration position when their storage is separated
--FILE--
<?php
// The storage is shared with the array that __serialize() or __debugInfo() hands
// out, then written to. It has a hole that the copy compacts, so positions have
// to follow the copy.

function make(string $class) {
    $object = new $class(['z' => 0, 'a' => 1, 'b' => 2, 'c' => 3, 'd' => 4]);
    unset($object['z']);
    return $object;
}

echo "-- write during foreach\n";
$it = make('ArrayIterator');
foreach ($it as $k => $v) {
    echo "$k=$v ";
    $data = $it->__serialize();
    $it[$k] = $v * 10;
}
echo "\n", json_encode($it->getArrayCopy()), ' ', json_encode($data[1]), "\n";

echo "-- write during manual iteration\n";
$it = make('ArrayIterator');
$it->rewind();
$it->next();
$data = $it->__serialize();
$it['e'] = 5;
for (; $it->valid(); $it->next()) {
    echo $it->key(), '=', $it->current(), ' ';
}
echo "\n", json_encode($data[1]), "\n";

echo "-- unset during foreach\n";
$it = make('ArrayIterator');
foreach ($it as $k => $v) {
    echo "$k=$v ";
    if ($k === 'b') {
        $data = $it->__debugInfo();
        unset($it['c']);
    }
}
echo "\n", json_encode($it->getArrayCopy()), ' ', json_encode(array_values($data)[0]), "\n";

echo "-- ArrayObject iterator, write through the ArrayObject\n";
$ao = make('ArrayObject');
foreach ($ao as $k => $v) {
    echo "$k=$v ";
    $data = $ao->__serialize();
    $ao[$k] = -$v;
}
echo "\n", json_encode($ao->getArrayCopy()), ' ', json_encode($data[1]), "\n";

echo "-- two ArrayObject iterators\n";
$ao = make('ArrayObject');
$it1 = $ao->getIterator();
$it2 = $ao->getIterator();
$it1->next();
$it2->next();
$it2->next();
$data = $ao->__serialize();
$ao['f'] = 6;
echo $it1->key(), ' ', $it2->key(), "\n";
$it1->next();
$it2->next();
echo $it1->key(), ' ', $it2->key(), "\n";

echo "-- iterator created after the separation\n";
$ao = make('ArrayObject');
$it1 = $ao->getIterator();
$it1->next();
$data = $ao->__serialize();
unset($ao['a']);
$it2 = $ao->getIterator();
echo $it1->key(), ' ', $it2->key(), "\n";
$data = $ao->__serialize();
$ao['g'] = 7;
$it1->next();
$it2->next();
echo $it1->key(), ' ', $it2->key(), "\n";

echo "-- foreach by reference\n";
$ao = make('ArrayObject');
$data = $ao->__serialize();
foreach ($ao as $k => &$v) {
    echo "$k=$v ";
    $v *= 2;
}
unset($v);
echo "\n", json_encode($ao->getArrayCopy()), ' ', json_encode($data[1]), "\n";

echo "-- nested foreach with writes\n";
$ao = make('ArrayObject');
foreach ($ao as $k1 => $v1) {
    foreach ($ao as $k2 => $v2) {
        if ($k2 === 'b') {
            $data = $ao->__serialize();
            $ao['b'] = $v2 + 1;
        }
    }
    echo "$k1=$v1 ";
}
echo "\n", json_encode($ao->getArrayCopy()), "\n";

echo "-- sort during foreach\n";
$ao = make('ArrayObject');
foreach ($ao as $k => $v) {
    echo "$k=$v ";
    if ($k === 'b') {
        $ao->ksort();
    }
}
echo "\n";

echo "-- storage replaced during iteration\n";
$ao = make('ArrayObject');
$it = $ao->getIterator();
$it->next();
$it->next();
$new = [];
foreach (['p', 'q', 'r', 's', 't'] as $k) {
    $new[$k] = $k;
}
$ao->exchangeArray($new);
// The position is kept as is
echo $it->key(), "\n";
// It then follows the new storage, here when filling it compacts it
unset($ao['p'], $ao['q']);
for ($i = 0; $i < 8; $i++) {
    $ao["k$i"] = $i;
}
echo $it->key(), "\n";
?>
--EXPECT--
-- write during foreach
a=1 b=2 c=3 d=4 
{"a":10,"b":20,"c":30,"d":40} {"a":10,"b":20,"c":30,"d":4}
-- write during manual iteration
b=2 c=3 d=4 e=5 
{"a":1,"b":2,"c":3,"d":4}
-- unset during foreach
a=1 b=2 d=4 
{"a":1,"b":2,"d":4} {"a":1,"b":2,"c":3,"d":4}
-- ArrayObject iterator, write through the ArrayObject
a=1 b=2 c=3 d=4 
{"a":-1,"b":-2,"c":-3,"d":-4} {"a":-1,"b":-2,"c":-3,"d":4}
-- two ArrayObject iterators
b c
c d
-- iterator created after the separation
b b
c c
-- foreach by reference
a=1 b=2 c=3 d=4 
{"a":2,"b":4,"c":6,"d":8} {"a":1,"b":2,"c":3,"d":4}
-- nested foreach with writes
a=1 b=3 c=3 d=4 
{"a":1,"b":6,"c":3,"d":4}
-- sort during foreach
a=1 b=2 c=3 d=4 
-- storage replaced during iteration
s
s
