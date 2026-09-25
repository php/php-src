--TEST--
Bug #71266 (Missing separation of properties HT in foreach etc)
--FILE--
<?php
$one = 1;
$two = 2;
$arr = ['foo' => $one, 'bar' => $two];
$obj = (object) $arr;
foreach ($obj as $val) {
    var_dump($val);
    $obj->bar = 42;
}

$arr = ['foo' => $one, 'bar' => $two];
$obj = (object) $arr;
next($obj);
var_dump(current($arr));
?>
--EXPECTF--
int(1)
int(42)

Deprecated: next(): Calling next() on an object is deprecated in %s on line %d
int(1)
