--TEST--
Bug #24098 (pathinfo() crash)
--FILE--
<?php
	var_dump(pathinfo("/dsds.asa"));
?>
--EXPECT--
array(3) {
  ["dirname"]=>
  string(1) "/"
  ["basename"]=>
  string(8) "dsds.asa"
  ["extension"]=>
  string(3) "asa"
}
