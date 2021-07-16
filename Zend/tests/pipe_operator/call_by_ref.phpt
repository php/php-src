--TEST--
Pipe handles reference variables the same as normal functions.
--FILE--
<?php

function _modify(int &$a): string {
    $a += 1;
    return "foo";
}

$a = 5;
$res1 = $a |> '_modify';

var_dump($res1);
var_dump($a);

try {
    $res2 = 5 |> '_modify';
} catch (Error $e) {
    print $e->getMessage() . PHP_EOL;
}

function &return_by_ref(string $s): string {
    $ret = $s . ' bar';
    return $ret;
}

function receive_by_ref(string &$b): string {
    $b .= " baz";
    print $b . PHP_EOL;
    return $b . ' beep';
}

$a = 'foo';
$res2 = $a |> 'return_by_ref' |> 'receive_by_ref';
var_dump($res2);

try {
    $not_defined |> '_modify';
} catch (Error $e) {
    print $e->getMessage() . PHP_EOL;
}

?>
--EXPECTF--
string(3) "foo"
int(6)
_modify(): Argument #1 ($a) cannot be passed by reference
foo bar baz
string(16) "foo bar baz beep"
_modify(): Argument #1 ($a) must be of type int, null given, called in %s on line %d
