--TEST--
list() with keys, evaluation order #3
--FILE--
<?php

$i = 0;
$a = [
    0 => [
        'b' => 'bar',
        'a' => 'foo',
    ],
    1 => 'a',
    3 => 'b',
];
list($a[$i++] => $a[$i++], $a[$i++] => $a[$i++]) = $a[$i++];
var_dump($i); // should be 5
var_dump($a[2]); // should be 'foo'
var_dump($a[4]); // should be 'bar'

?>
--EXPECT--
int(5)
string(3) "foo"
string(3) "bar"
