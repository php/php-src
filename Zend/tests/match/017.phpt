--TEST--
Test strict comparison with match expression jump table
--FILE--
<?php

function wrong() {
    throw new Exception();
}

function test_int($char) {
    return match ($char) {
        0 => wrong(),
        1 => wrong(),
        2 => wrong(),
        3 => wrong(),
        4 => wrong(),
        5 => wrong(),
        6 => wrong(),
        7 => wrong(),
        8 => wrong(),
        9 => wrong(),
        default => 'Not matched',
    };
}

foreach (range(0, 9) as $int) {
    var_dump((string) $int);
    var_dump(test_int((string) $int));
}

function test_string($int) {
    return match ($int) {
        '0' => wrong(),
        '1' => wrong(),
        '2' => wrong(),
        '3' => wrong(),
        '4' => wrong(),
        '5' => wrong(),
        '6' => wrong(),
        '7' => wrong(),
        '8' => wrong(),
        '9' => wrong(),
        default => 'Not matched',
    };
}

foreach (range(0, 9) as $int) {
    var_dump($int);
    var_dump(test_string($int));
}

?>
--EXPECT--
string(1) "0"
string(11) "Not matched"
string(1) "1"
string(11) "Not matched"
string(1) "2"
string(11) "Not matched"
string(1) "3"
string(11) "Not matched"
string(1) "4"
string(11) "Not matched"
string(1) "5"
string(11) "Not matched"
string(1) "6"
string(11) "Not matched"
string(1) "7"
string(11) "Not matched"
string(1) "8"
string(11) "Not matched"
string(1) "9"
string(11) "Not matched"
int(0)
string(11) "Not matched"
int(1)
string(11) "Not matched"
int(2)
string(11) "Not matched"
int(3)
string(11) "Not matched"
int(4)
string(11) "Not matched"
int(5)
string(11) "Not matched"
int(6)
string(11) "Not matched"
int(7)
string(11) "Not matched"
int(8)
string(11) "Not matched"
int(9)
string(11) "Not matched"
