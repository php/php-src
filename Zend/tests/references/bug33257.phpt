--TEST--
Bug #33257 (array_splice() inconsistent when passed function instead of variable)
--INI--
error_reporting=4095
--FILE--
<?php
class X {
  protected static $arr = array("a", "b", "c");
  public static function getArr() {
    return self::$arr;
  }
}

//$arr1 = X::getArr();
array_splice(X::getArr(), 1, 1);
print_r(X::getArr());
?>
--EXPECTF--
Notice: Only variables should be passed by reference in %sbug33257.php on line 10
Array
(
    [0] => a
    [1] => b
    [2] => c
)
