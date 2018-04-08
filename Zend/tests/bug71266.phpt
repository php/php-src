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
--EXPECT--
int(1)
int(42)
int(1)
