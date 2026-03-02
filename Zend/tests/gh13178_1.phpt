--TEST--
GH-13178: Packed to hash must reset iterator position
--FILE--
<?php
$array = ['foo'];
foreach ($array as $key => &$value) {
    var_dump($key);
    unset($array[$key]);
    $array[] = 'foo';
    if ($key === 10) {
        break;
    }
}
?>
--EXPECT--
int(0)
int(1)
int(2)
int(3)
int(4)
int(5)
int(6)
int(7)
int(8)
int(9)
int(10)
