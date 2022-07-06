--TEST--
Bug #38377 (session_destroy() gives warning after session_regenerate_id())
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
session_start();
session_regenerate_id();
session_destroy();
echo "Done\n";
?>
--EXPECT--
Done
