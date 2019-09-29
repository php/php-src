--TEST--
Bug #78015: Incorrect evaluation of expressions involving partials array in SCCP
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$x = 1;

function test1() {
    global $x;
    $a = ['b' => [$x], 'c' => [$x]];
    $d = $a['b'] + $a['c'];
    return $d;
}

function test2() {
    global $x;
    $a = ['b' => [$x]];
    $d = !$a['b'];
    return $d;
}

function test3() {
    global $x;
    $a = ['b' => [$x]];
    $d = (int) $a['b'];
    return $d;
}

function test4() {
    global $x;
    $a = ['b' => [$x]];
    $d = $a['b'] ?: 42;
    return $d;
}

function test5() {
    global $x;
    $a = ['b' => [$x]];
    $d = is_array($a['b']);
    return $d;
}

function test6() {
    global $x;
    $a = ['b' => [$x]];
    $b = "foo";
    $d = "$a[b]{$b}bar";
    return $d;
}

function test7() {
    global $x;
    $a = ['b' => [$x]];
    $y = 1;
    foreach ($a['b'] as $_) {
        $y = 2;
    }
    return $y;
}

function test8($array) {
    $i = 0;
    $ret = [[]];
    foreach ($array as $_) {
      $i++;
      $ret = [[
        'x' => 0,
        'y' => $i,
      ]];
    }
    return $ret[0];
}

function test9() {
    global $x;
    $a = ['b' => [$x]];
    return serialize($a['b']);
}

var_dump(test1());
var_dump(test2());
var_dump(test3());
var_dump(test4());
var_dump(test5());
var_dump(test6());
var_dump(test7());
var_dump(test8([1]));
var_dump(test9());

?>
--EXPECTF--
array(1) {
  [0]=>
  int(1)
}
bool(false)
int(1)
array(1) {
  [0]=>
  int(1)
}
bool(true)

Notice: Array to string conversion in %s on line %d
string(11) "Arrayfoobar"
int(2)
array(2) {
  ["x"]=>
  int(0)
  ["y"]=>
  int(1)
}
string(14) "a:1:{i:0;i:1;}"
