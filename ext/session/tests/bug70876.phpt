--TEST--
Bug #70876 Segmentation fault when regenerating session id with strict mode
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
html_errors=0
session.save_handler=files
--FILE--
<?php
ini_set('session.use_strict_mode', true);
session_start();
session_regenerate_id();

echo "ok";
?>
--EXPECT--
ok
