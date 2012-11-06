--TEST--
Bug #63389 (Missing context check on libxml_set_streams_context() causes memleak)
--SKIPIF--
<?php if (!extension_loaded('libxml')) die('skip'); ?>
--FILE--
<?php
$fp = fopen("php://input", "r");
libxml_set_streams_context($fp);
libxml_set_streams_context("a");
echo "okey";
?>
--EXPECTF--
Warning: libxml_set_streams_context() expects parameter 1 to be resource, string given in %sbug63389.php on line %d
okey
