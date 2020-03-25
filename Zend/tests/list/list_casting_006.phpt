--TEST--
"Cast during unpacking - foreach loop" list()
--FILE--
<?php
$arr = [[1, 2, 3], [4, 5, 6]];

foreach ($arr as list((string) $a, (bool) $b, $c)) {
    var_dump($a, $b, $c);
}

foreach ($arr as [(string) $a, (bool) $b, $c]) {
    var_dump($a, $b, $c);
}
?>
--EXPECT--
string(1) "1"
bool(true)
int(3)
string(1) "4"
bool(true)
int(6)
string(1) "1"
bool(true)
int(3)
string(1) "4"
bool(true)
int(6)
