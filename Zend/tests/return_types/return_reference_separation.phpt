--TEST--
Return value separation
--FILE--
<?php
function test1(&$abc) : string {
    return $abc;
}

function &test2(int $abc) : string {
    return $abc;
}

function &test3(int &$abc) : string {
    return $abc;
}

$a = 123;

var_dump(test1($a));
var_dump($a);
var_dump(test2($a));
var_dump($a);
var_dump(test3($a));
var_dump($a);

?>
--EXPECT--
string(3) "123"
int(123)
string(3) "123"
int(123)
string(3) "123"
string(3) "123"
