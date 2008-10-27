--TEST--
Bug #20539 (PHP CLI Segmentation Fault)
--SKIPIF--
<?php if (!extension_loaded("session")) die("skip session extension not available"); ?>
<?php unlink(__DIR__. '/sess_' .session_id()); ?>
--INI--
session.auto_start=1
session.save_handler=files
session.save_path=./tests/basic/
--FILE--
<?php
	print "good :)\n";
	$filename = __DIR__ . '/sess_' . session_id();
	var_dump(unlink($filename));
?>
--EXPECT--
good :)
bool(true)

