--TEST--
GH-24792: Assignment results can still be read and passed by reference
--FILE--
<?php
function f(&$x) {
    $x[] = 9;
}

$arr = [[1]];
f([&$x] = $arr);
var_dump($arr[1]);

$b = [1];
f($a =& $b);
var_dump($b[1]);

var_dump(($v = ['k' => 'v'])['k'], $v);
var_dump(isset(($w = [1])[0]), ($w = [1])[5] ?? 'default', empty(($w = [1])[0]));

$o = new stdClass;
$o->p = [1];
var_dump(($c = $o)->p[0]);
?>
--EXPECT--
int(9)
int(9)
string(1) "v"
array(1) {
  ["k"]=>
  string(1) "v"
}
bool(true)
string(7) "default"
bool(false)
int(1)
