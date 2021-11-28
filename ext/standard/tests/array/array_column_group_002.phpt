--TEST--
Group an element using string index
--FILE--
<?php

$ar = [
    ['id' => 1, 'name' => 'hassan', 'score' => 'A', 's' => '1'],
    ['id' => 2, 'name' => 'hassan', 'score' => 'B', 's' => '2'],
    ['id' => 3, 'name' => 'ahmed', 'score' => 'C', 's' => '1'],
    ['id' => 4, 'name' => 'moustafa', 'score' => 'D', 's' => '2'],
    ['id' => 5, 'name' => 'hassan', 'score' => 'A', 's' => '3'],
    ['id' => 6, 'name' => 'wael', 'score' => 'A', 's' => '3'],
    ['id' => 6, 'name' => 'wael', 'score' => 'D', 's' => '1'],
];

print_r(array_column($ar, 'name', 'score', true));

?>
--EXPECT--
Array
(
    [A] => Array
        (
            [0] => hassan
            [1] => hassan
            [2] => wael
        )

    [B] => Array
        (
            [0] => hassan
        )

    [C] => Array
        (
            [0] => ahmed
        )

    [D] => Array
        (
            [0] => moustafa
            [1] => wael
        )

)
