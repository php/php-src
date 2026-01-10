--TEST--
GH-13178: Packed to hash must reset nInternalPointer
--FILE--
<?php
$array = ['foo'];
reset($array);
while (true) {
    $key = key($array);
    next($array);
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
