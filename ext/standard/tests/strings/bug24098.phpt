--TEST--
Bug #24098 (pathinfo() crash)
--FILE--
<?php
	var_dump(pathinfo("/dsds.asa"));
?>
--EXPECTF--
array(4) {
  ["dirname"]=>
  string(1) "%e"
  ["basename"]=>
  string(8) "dsds.asa"
  ["extension"]=>
  string(3) "asa"
  ["filename"]=>
  string(4) "dsds"
}
