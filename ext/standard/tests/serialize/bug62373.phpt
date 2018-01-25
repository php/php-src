--TEST--
Bug #62373 (serialize() generates wrong reference to the object)
--FILE--
<?php
class A {}
class B {}

$size_of_ce = (((int)(log(PHP_INT_MAX) / log(2)) + 1 == 32 ? 368: 680) + 15) & ~15;
$dummy = array();
$b = new B();
$period = $size_of_ce << 5;
for ($i = 0; $i < $period * 3; $i++) {
    $a = new A();
    $s = unserialize(serialize(array($b, $a)));
    if ($s[0] === $s[1]) {
        echo "OOPS\n";
        break;
    }
    $dummy[] = $a;
}

echo "OK\n";
?>
--EXPECT--
OK
