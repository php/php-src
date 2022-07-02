--TEST--
Bug #30862 (Static array with boolean indexes)
--FILE--
<?php
class T {
    static $a = array(false=>"false", true=>"true");
}
print_r(T::$a);
?>
----------
<?php
define("X",0);
define("Y",1);
class T2 {
    static $a = array(X=>"false", Y=>"true");
}
print_r(T2::$a);
?>
--EXPECT--
Array
(
    [0] => false
    [1] => true
)
----------
Array
(
    [0] => false
    [1] => true
)
