--TEST--
Bug #20539 (PHP CLI Segmentation Fault)
--INI--
session.auto_start=1
--FILE--
<?php
	print "good :)";
?>
--EXPECT--
good :)
