--TEST--
Bug #33257 array_splice() inconsistent when passed function instead of variable 
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
Fatal error: Only variables can be passed by reference in %sbug33257.php on line 10
