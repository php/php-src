--TEST--
GH-11244: Modifying a copied by-ref iterated array resets the array position (multiple copies)
--FILE--
<?php

$data = [0, 1, 2];

foreach ($data as $key => &$value) {
    echo "$value\n";
    if ($value === 1) {
        $cow_copy = [$data, $data, $data];
        echo "unset $value\n";
        unset($cow_copy[0][$key]);
        unset($data[$key]);
        unset($cow_copy[2][$key]);
    }
}

print_r($cow_copy);

?>
--EXPECTF--
0
1
unset 1
2
Array
(
    [0] => Array
        (
            [0] => 0
            [2] => 2
        )

    [1] => Array
        (
            [0] => 0
            [1] => 1
            [2] => 2
        )

    [2] => Array
        (
            [0] => 0
            [2] => 2
        )

)
