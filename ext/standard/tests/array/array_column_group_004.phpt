--TEST--
Group the whole array using string index
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

print_r(array_column_group($ar, null, 'name'));
?>
--EXPECT--
Array
(
    [hassan] => Array
        (
            [0] => Array
                (
                    [id] => 1
                    [name] => hassan
                    [score] => A
                    [s] => 1
                )

            [1] => Array
                (
                    [id] => 2
                    [name] => hassan
                    [score] => B
                    [s] => 2
                )

            [2] => Array
                (
                    [id] => 5
                    [name] => hassan
                    [score] => A
                    [s] => 3
                )

        )

    [ahmed] => Array
        (
            [0] => Array
                (
                    [id] => 3
                    [name] => ahmed
                    [score] => C
                    [s] => 1
                )

        )

    [moustafa] => Array
        (
            [0] => Array
                (
                    [id] => 4
                    [name] => moustafa
                    [score] => D
                    [s] => 2
                )

        )

    [wael] => Array
        (
            [0] => Array
                (
                    [id] => 6
                    [name] => wael
                    [score] => A
                    [s] => 3
                )

            [1] => Array
                (
                    [id] => 6
                    [name] => wael
                    [score] => D
                    [s] => 1
                )

        )

)
