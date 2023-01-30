--TEST--
SplFileObject::fpassthru function - functionality test with length
--FILE--
<?php
$obj = New SplFileObject(__DIR__.'/SplFileObject_testinput.csv');
$read = [];
$read[] = $obj->fpassthru(1);
echo "\n";
$read[] = $obj->fpassthru(10);
echo "\n";
$read[] = $obj->fpassthru(0);
echo "\n";
$read[] = $obj->fpassthru(-10000);
echo "\n";
print_r($read);
?>
--EXPECT--
f
irst,secon

d,third
1,2,3
4,5,6
7,8,9
0,0,0

Array
(
    [0] => 1
    [1] => 10
    [2] => 0
    [3] => 32
)
