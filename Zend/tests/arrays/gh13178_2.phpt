--TEST--
GH-13178: Unsetting last offset must floor iterator position
--FILE--
<?php
$array = [100 => 'foo'];
foreach ($array as $key => &$value) {
    var_dump($key);
    unset($array[$key]);
    $array[] = 'foo';
    if ($key === 110) {
        break;
    }
}
?>
--EXPECT--
int(100)
int(101)
int(102)
int(103)
int(104)
int(105)
int(106)
int(107)
int(108)
int(109)
int(110)
