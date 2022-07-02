--TEST--
Bug #20539 (PHP CLI Segmentation Fault)
--EXTENSIONS--
session
--SKIPIF--
<?php if (file_exists(__DIR__. '/sess_' .session_id())) unlink(__DIR__. '/sess_' .session_id()); ?>
--INI--
session.auto_start=1
session.save_handler=files
session.save_path=./tests/basic/
--FILE--
<?php
    print "good :)\n";
    $filename = __DIR__ . '/sess_' . session_id();
    var_dump(file_exists($filename));
    @unlink($filename);
?>
--EXPECT--
good :)
bool(true)
