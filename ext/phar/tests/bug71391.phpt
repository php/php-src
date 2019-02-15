--TEST--
Phar: bug #71391: NULL Pointer Dereference in phar_tar_setupmetadata()
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
// duplicate since the tar will change
copy(__DIR__."/bug71391.tar", __DIR__."/bug71391.test.tar");
$p = new PharData(__DIR__."/bug71391.test.tar");
$p->delMetaData();
?>
DONE
--CLEAN--
<?php
unlink(__DIR__."/bug71391.test.tar");
?>
--EXPECT--
DONE
