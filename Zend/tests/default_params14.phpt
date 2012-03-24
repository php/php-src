--TEST--
Default parameters - 14, FAST_ZPP
--FILE--
<?php
var_dump(array_slice(["a" => 1, "b" => 2, "c" => 3, "d" => 4], 2, 2, true));
var_dump(array_slice(["a" => 1, "b" => 2, "c" => 3, "d" => 4], 2, default, true));
echo "Done\n";
?>
--EXPECTF--
array(2) {
  ["c"]=>
  int(3)
  ["d"]=>
  int(4)
}
array(2) {
  ["c"]=>
  int(3)
  ["d"]=>
  int(4)
}
Done
