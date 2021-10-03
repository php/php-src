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
?>
--EXPECT--
0
1
2
3
{"0":true,"3":true}
