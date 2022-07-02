--TEST--
Test match with mixed int/string jumptable
--FILE--
<?php

function test($value) {
    echo match ($value) {
        1 => '1 int',
        '1' => '1 string',
        2 => '2 int',
        '2' => '2 string',
        3 => '3 int',
        '3' => '3 string',
        4 => '4 int',
        '4' => '4 string',
        5 => '5 int',
        '5' => '5 string',
    };
    echo "\n";
}

test(1);
test('1');
test(2);
test('2');
test(3);
test('3');
test(4);
test('4');
test(5);
test('5');

?>
--EXPECT--
1 int
1 string
2 int
2 string
3 int
3 string
4 int
4 string
5 int
5 string
