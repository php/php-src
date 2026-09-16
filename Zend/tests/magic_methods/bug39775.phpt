--TEST--
Bug #39775 ("Indirect modification ..." message is not shown)
--FILE--
<?php
class test {
    var $array = array();
    function __get($var) {
        $v =& $this->array;
        return $this->array;
    }
}
$t = new test;
$t->anything[] = 'bar';
print_r($t->anything);
?>
--EXPECTF--
Notice: Indirect modification of overloaded property test::$anything has no effect in %sbug39775.php on line 10
Array
(
)
