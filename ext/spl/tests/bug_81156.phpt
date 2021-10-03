--TEST--
Bug #81156 	unset() on ArrayObject inside foreach skips next index
--FILE--
<?php

$ao = new ArrayObject([true, false, false, true]);

foreach ($ao as $key => $value) {
    echo $key, "\n";
    if (!$value) {
        $ao->offsetUnset($key);
    }
}
echo json_encode($ao), "\n";

$ao = new ArrayObject([true, false, false, true]);
$ao->offsetUnset(2);
foreach ($ao as $key => $value) {
    echo $key, "\n";
    if (!$value) {
        $ao->offsetUnset($key);
    }
}
echo json_encode($ao), "\n";
?>
--EXPECT--
0
1
2
3
{"0":true,"3":true}
0
1
3
{"0":true,"3":true}
