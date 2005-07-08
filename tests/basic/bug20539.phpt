--TEST--
Bug #20539 (PHP CLI Segmentation Fault)
--INI--
session.auto_start=1
session.save_handler=files
--FILE--
<?php
	print "good :)\n";
?>
--EXPECT--
good :)
