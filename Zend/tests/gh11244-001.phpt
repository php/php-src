--TEST--
GH-11244: Modifying a copied by-ref iterated array resets the array position (packed)
--FILE--
<?php

$data = [0, 1, 2];

foreach ($data as $key => &$value) {
    echo "$value\n";
    if ($value === 1) {
        $cow_copy = $data;
        echo "unset $value\n";
        unset($data[$key]);
    }
}

?>
--EXPECTF--
0
1
unset 1
2
