--TEST--
Bug #20539 (PHP CLI Segmentation Fault)
--SKIPIF--
<?php if (!extension_loaded("session")) die("skip session extension not available"); ?>
--INI--
session.auto_start=1
session.save_handler=files
session.save_path=./tests/basic/
--FILE--
<?php
	print "good :)\n";
	unlink(dirname(__FILE__) . '/sess_' . session_id());
?>
--EXPECT--
good :)
