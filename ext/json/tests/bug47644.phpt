--TEST--
Bug #47644 (valid large integers are truncated)
--SKIPIF--
<?php
if (!extension_loaded('json')) die('skip');
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php

for ($i = 10000000000000000; $i < 10000000000000006; $i++) {
	var_dump(json_decode("[$i]"));
}


echo "Done\n";
?>
--EXPECT--	
array(1) {
  [0]=>
  int(10000000000000000)
}
array(1) {
  [0]=>
  int(10000000000000001)
}
array(1) {
  [0]=>
  int(10000000000000002)
}
array(1) {
  [0]=>
  int(10000000000000003)
}
array(1) {
  [0]=>
  int(10000000000000004)
}
array(1) {
  [0]=>
  int(10000000000000005)
}
Done
