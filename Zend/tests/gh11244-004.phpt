--TEST--
GH-11244: Modifying a copied by-ref iterated array resets the array position (with object)
--FILE--
<?php

$obj = (object)[1,2,3];

foreach ($obj as $p => $v) {
    echo "$p : $v\n";
    $clone = clone $obj;
    $ref = &$obj->$p;
}

?>
--EXPECTF--
0 : 1
1 : 2
2 : 3
