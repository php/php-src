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
  [u"dirname"]=>
  unicode(1) "/"
  [u"basename"]=>
  unicode(8) "dsds.asa"
  [u"extension"]=>
  unicode(3) "asa"
  [u"filename"]=>
  unicode(4) "dsds"
}
