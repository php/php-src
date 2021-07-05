--TEST--
Pipe operator accepts by-reference functions
--FILE--
<?php

function _modify(int &$a): string {
    $a += 1;
    return "foo";
}

//try {
$a = 5;
$res1 = $a |> '_modify';

var_dump($res1);
var_dump($a);
?>
--EXPECT--
string(3) "foo"
int(6)
