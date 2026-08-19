--TEST--
unserialize() with wrong-case class name fails with wrong case
--FILE--
<?php
class MyRow {
    public string $name = '';
}

// Correct case — no deprecation
$obj = unserialize('O:5:"MyRow":1:{s:4:"name";s:5:"hello";}');
echo get_class($obj) . "\n";

// Wrong case — E_DEPRECATED
$obj2 = unserialize('O:5:"MYROW":1:{s:4:"name";s:5:"hello";}');
echo get_class($obj2) . "\n";

$obj3 = unserialize('O:5:"myrow":1:{s:4:"name";s:5:"hello";}');
echo get_class($obj3) . "\n";
?>
--EXPECTF--
MyRow
__PHP_Incomplete_Class
__PHP_Incomplete_Class
