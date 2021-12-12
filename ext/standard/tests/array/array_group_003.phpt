--TEST--
Group an element using an integer but casted into a string
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
print_r(array_group($ar, 'score', 's'));
?>
--EXPECT--
Array
(
    [1] => Array
        (
            [0] => A
            [1] => C
            [2] => D
        )

    [2] => Array
        (
            [0] => B
            [1] => D
        )

    [3] => Array
        (
            [0] => A
            [1] => A
        )

)
