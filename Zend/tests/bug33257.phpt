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
try {
	array_splice(X::getArr(), 1, 1);
} catch (Throwable $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}
print_r(X::getArr());
?>
--EXPECT--
Exception: Cannot pass parameter 1 by reference
Array
(
    [0] => a
    [1] => b
    [2] => c
)
