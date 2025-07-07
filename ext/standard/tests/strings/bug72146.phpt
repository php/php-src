--TEST--
Bug #72146 (Integer overflow on substr_replace)
--FILE--
<?php
var_dump(substr_replace(["ABCDE"], "123", 3, ((2 ** (PHP_SYS_SIZE * 8 - 2) - 1) << 1) + 1));
?>
--EXPECT--
array(1) {
  [0]=>
  string(6) "ABC123"
}
