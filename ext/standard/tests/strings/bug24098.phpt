--TEST--
Bug #24098 (pathinfo() crash)
--SKIPIF--
<?php if (DIRECTORY_SEPARATOR == '\\') die("skip directory separator won't match expected output"); ?>
--FILE--
<?php
	var_dump(pathinfo("/dsds.asa"));
?>
--EXPECT--
array(4) {
  ["dirname"]=>
  string(1) "/"
  ["basename"]=>
  string(8) "dsds.asa"
  ["extension"]=>
  string(3) "asa"
  ["filename"]=>
  string(4) "dsds"
}
